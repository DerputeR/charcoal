#include "renderer.h"
#include "SDL3/SDL_log.h"
#include "shader_loader.h"
#include <cmath>
#include <cstddef>
#include <format>
#include <glm/gtc/integer.hpp>
#include <glm/vec4.hpp>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f}, rgb{0xFFFFFF} {
}

Vertex::Vertex(const glm::vec3 &position) : position{position}, rgb{0xFFFFFF} {
}

Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &rgb) :
        position{position}, rgb{(glm::uround(rgb.x * 255.0f) << 16) |
                                    (glm::uround(rgb.y * 255.0f) << 8) |
                                    glm::uround(rgb.z * 255.0f)} {
    SDL_LogDebug(
            SDL_LOG_CATEGORY_RENDER, "New vertex with color %06X", this->rgb);
    glm::vec4 glsl{((this->rgb & 0xFF0000u) >> 16) / 255.0f,
            ((this->rgb & 0x00FF00u) >> 8) / 255.0f,
            (this->rgb & 0x0000FFu) / 255.0f, 1.0f};

    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER,
            "In GLSL, this would be read as: r: %f; g: %f; b: %f", glsl.x,
            glsl.y, glsl.z);
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb) :
        position{position}, rgb{rgb} {
}

void Vertex::set_rgb(int r, int g, int b) {
    rgb = (r << 16) | (g << 8) | b;
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
}

void Renderer::load_attribute_indices() {
    if (shader_program == 0) {
        position_index = -1;
        rgb_index = -1;
    } else {
        position_index = glGetAttribLocation(shader_program, "pos");
        rgb_index = glGetAttribLocation(shader_program, "rgb");
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
    } else {
        // attrib index, attrib element count, attrib element type, normalized,
        // size of vertex (stride), attrib offset within vertex
        glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE,
                sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, position)));
        glVertexAttribIPointer(rgb_index, 1, GL_UNSIGNED_INT, sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, rgb)));
        glEnableVertexAttribArray(position_index);
        glEnableVertexAttribArray(rgb_index);
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
