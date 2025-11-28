#include "renderer.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include "app_state.h"
#include "shader_loader.h"
#include <cmath>
#include <cstddef>
#include <format>
#include <glm/gtc/integer.hpp>
#include <glm/vec4.hpp>
#include <algorithm>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f}, rgb{0xFFFFFF}, uv{0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) :
        position{position}, rgb{0xFFFFFF}, uv{position.x, position.y} {
}

// TODO: clamp the rgb values
Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &rgb) :
        position{position}, rgb{pack_normalized_rgb24_to_uint32(rgb)},
        uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb) :
        position{position}, rgb{rgb}, uv{position.x, position.y} {
}

Vertex::Vertex(
        const glm::vec3 &position, const glm::vec3 &rgb, const glm::vec2 &uv) :
        position{position}, rgb{pack_normalized_rgb24_to_uint32(rgb)}, uv{uv} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb,
        const glm::vec2 &uv) : position{position}, rgb{rgb}, uv{uv} {
}

void Vertex::set_rgb(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    rgb = (r << 16) | (g << 8) | b;
}

glm::uint32 Vertex::pack_rgb24_to_uint32(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    glm::uint32 rgb = (r << 16) | (g << 8) | b;
    return rgb;
}

glm::uint32 Vertex::pack_normalized_rgb24_to_uint32(float r, float g, float b) {
    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);
    int ri = glm::uround(r * 255.0f);
    int gi = glm::uround(g * 255.0f);
    int bi = glm::uround(b * 255.0f);
    return pack_rgb24_to_uint32(ri, gi, bi);
}

glm::uint32 Vertex::pack_normalized_rgb24_to_uint32(const glm::vec3 rgb) {
    return pack_normalized_rgb24_to_uint32(rgb.r, rgb.g, rgb.b);
}

Renderer::Renderer() :
        Renderer(Shader::ShaderLoader::create_program(
                Shader::ShaderLoader::DEFAULT_VERT_SRC,
                Shader::ShaderLoader::DEFAULT_FRAG_SRC)) {
}

Renderer::Renderer(GLuint shader_program) :
        shader_program{0}, error{Error::none}, error_msg{""}, vbo{0}, vao{0},
        index_count{0}, position_index{-1}, rgb_index{-1} {
    set_shader_program(shader_program);
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    // face must be front and back. mode can be fill or wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // WIP: textures
    glGenTextures(1, &texture); // this can be an array of textures
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load the texture data
    constexpr const char *crate_path = "resources/textures/crate.png";
    SDL_Surface *surface = IMG_Load(crate_path);
    if (surface == nullptr) {
        SDL_LogCritical(
                SDL_LOG_CATEGORY_SYSTEM, "Unable to load \"%s\"", crate_path);
    } else {
        SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_DestroySurface(temp);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void Renderer::load_attribute_indices() {
    if (shader_program == 0) {
        position_index = -1;
        rgb_index = -1;
        uv_index = -1;
    } else {
        position_index = glGetAttribLocation(shader_program, "pos");
        rgb_index = glGetAttribLocation(shader_program, "rgb");
        uv_index = glGetAttribLocation(shader_program, "uv");
    }
}

void Renderer::set_shader_program(GLuint program) {
    shader_program = program;
    if (shader_program == 0) {
        error = Error::invalid_program;
        error_msg = "Invalid shader program loaded";
    }
    load_attribute_indices();
}

void Renderer::submit_mesh(const Mesh &mesh) {
    // bind the VAO so we can update its state
    glBindVertexArray(vao);

    // copy verts to the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.verts.size() * sizeof(Vertex),
            mesh.verts.data(), GL_STATIC_DRAW);

    // check if the data was uploaded correctly
    GLint buf_size = 0;
    std::size_t expected_size = mesh.verts.size() * sizeof(Vertex);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
    if (static_cast<std::size_t>(buf_size) != expected_size) {
        error = Error::invalid_vbo;
        error_msg = std::format("VBO buffer size {} was expected to be size {}",
                buf_size, expected_size);
        glDisableVertexAttribArray(position_index);
        glDisableVertexAttribArray(rgb_index);
        glDisableVertexAttribArray(uv_index);
    } else {
        // attrib index, attrib element count, attrib element type,
        // normalized, size of vertex (stride), attrib offset within vertex
        glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, position)));
        glVertexAttribIPointer(rgb_index, 1, GL_UNSIGNED_INT, sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, rgb)));
        glVertexAttribPointer(uv_index, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, uv)));
        glEnableVertexAttribArray(position_index);
        glEnableVertexAttribArray(rgb_index);
        glEnableVertexAttribArray(uv_index);
    }

    // copy the indices to the ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(int),
            mesh.indices.data(), GL_STATIC_DRAW);

    // check if the data was uploaded correctly
    buf_size = 0;
    expected_size = mesh.indices.size() * sizeof(int);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
    if (static_cast<std::size_t>(buf_size) != expected_size) {
        error = Error::invalid_ebo;
        error_msg = std::format("EBO buffer size {} was expected to be size {}",
                buf_size, expected_size);
        index_count = 0;
    } else {
        index_count = mesh.indices.size();
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::render(AppState *app_state) {
    if (error == Error::none) {
        glUseProgram(shader_program);
        if (app_state != nullptr) {
            // TODO: abstract uniform updating. might need a separate shader
            // program class to define this behavior
            float time_value =
                    app_state->time.ns_to_f32(app_state->time.get_total_time());
            float offset = (std::sin(time_value) / 2.0);
            int vertex_offset_location =
                    glGetUniformLocation(shader_program, "offset");
            if (vertex_offset_location > -1) {
                glUniform1f(vertex_offset_location, offset);
            }
        }

        // TODO: figure out where this should go
        // int texture_location = glGetUniformLocation(shader_program,
        // "obj_texture"); if (texture_location > -1) {
        //     glUniform1f(texture_location, texture);
        // }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &vbo);
}

Renderer::Error Renderer::get_error() const {
    return error;
}

std::string Renderer::get_error_msg() const {
    return error_msg;
}

void Renderer::set_error(Error e, const std::string &msg) {
    error = e;
    error_msg = msg;
}

void Renderer::clear_error() {
    set_error(Error::none, "");
}
} // namespace Charcoal
