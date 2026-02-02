#include "renderer.h"
#include "scene.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "app_state.h"
#include "color.h"
#include "shader_loader.h"
#include "vertex.h"
#include <cmath>
#include <cstddef>
#include <format>
#include <glm/gtc/type_ptr.hpp>

namespace Charcoal {
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

    // TODO: WIP: textures
    constexpr const char *paths[2] = {"./resources/textures/crate.png",
            "./resources/textures/glass.png"};
    glGenTextures(2, &texture[0]); // this can be an array of textures
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // load the texture data
        SDL_Surface *initial_load = nullptr;
        initial_load = SDL_LoadPNG(paths[i]);
        // initial_load = IMG_Load(paths[i]);
        if (initial_load == nullptr) {
            SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM,
                    "Unable to load \"%s\": %s", paths[i], SDL_GetError());
            initial_load = SDL_CreateSurface(
                    2, 2, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA32);
            glm::uint32 *pixels =
                    static_cast<glm::uint32 *>(initial_load->pixels);
            pixels[0] = Color::pack_rgba32(255, 0, 255, 255);
            pixels[1] = Color::pack_rgba32(0, 0, 0, 255);
            pixels[2] = Color::pack_rgba32(0, 0, 0, 255);
            pixels[3] = Color::pack_rgba32(255, 0, 255, 255);
        }
        SDL_Surface *converted =
                SDL_ConvertSurface(initial_load, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(initial_load);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, converted->w, converted->h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, converted->pixels);
        SDL_DestroySurface(converted);
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
                reinterpret_cast<GLvoid *>(offsetof(Vertex, color)));
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
            if (app_state->time.get_frame_count() <= 1) {
                // Assign the texture units to the samplers. this only needs to
                // be done once before rendering
                int obj_texture_loc =
                        glGetUniformLocation(shader_program, "obj_texture");
                if (obj_texture_loc > -1) {
                    glUniform1i(obj_texture_loc, 0);
                }

                int glass_texture_loc =
                        glGetUniformLocation(shader_program, "glass_texture");
                if (glass_texture_loc > -1) {
                    glUniform1i(glass_texture_loc, 1);
                }
            }

            // TODO: abstract uniform updating. might need a separate shader
            // program class to define this behavior
            float time_value =
                    app_state->time.ns_to_f32(app_state->time.get_total_time());
            int transform_location =
                    glGetUniformLocation(shader_program, "transform");
            if (transform_location > -1) {
                Scene *ts = app_state->scene.get();
                glm::mat4 transform = ts->get_local_transform_matrix();
                glUniformMatrix4fv(transform_location, 1, GL_FALSE,
                        glm::value_ptr(transform));
            }
            int blend_location = glGetUniformLocation(shader_program, "blend");
            if (blend_location > -1) {
                glUniform1f(blend_location,
                        0.5f + (std::sin(time_value * 2.0f) / 2.0));
            }
        }

        // TODO: figure out where this should go
        // int texture_location = glGetUniformLocation(shader_program,
        // "obj_texture"); if (texture_location > -1) {
        //     glUniform1f(texture_location, texture);
        // }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture[1]);

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
