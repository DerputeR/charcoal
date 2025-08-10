#include "renderer.h"
#include "shader_loader.h"
#include <cstddef>
#include <format>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) : position{position} {
}

Renderer::Renderer() :
        Renderer(ShaderLoader::create_program(ShaderLoader::DEFAULT_VERT_SRC,
                ShaderLoader::DEFAULT_FRAG_SRC)) {
}

Renderer::Renderer(GLuint shader_program) :
        shader_program{shader_program}, error{Error::none}, error_msg{""},
        vbo{0}, vao{0}, vert_count{0} {
    if (shader_program == 0) {
        error = Error::invalid_program;
        error_msg = "Invalid shader program loaded";
    }

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
}

void Renderer::set_shader_program(GLuint program) {
    shader_program = program;
}

void Renderer::submit_verts(const std::vector<Vertex> &verts) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);

    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(),
            GL_STATIC_DRAW);

    // check if the data was uploaded correctly
    GLint buf_size{0};
    std::size_t expected_size{verts.size() * sizeof(Vertex)};
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
    if (static_cast<std::size_t>(buf_size) != expected_size) {
        error = Error::invalid_vbo;
        error_msg = std::format("VBO buffer size {} was expected to be size {}",
                buf_size, expected_size);
        vert_count = 0;
        glDisableVertexAttribArray(vao);
    } else {
        vert_count = verts.size();
        // glVertexAttribPointer(
        //         0, 1, GL_FLOAT_VEC3, GL_FALSE, sizeof(Vertex), nullptr);
        GLint position_index = glGetAttribLocation(shader_program, "position");
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                reinterpret_cast<GLvoid *>(offsetof(Vertex, position)));
        glEnableVertexAttribArray(vao);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::render() {
    if (error == Error::none) {
        glUseProgram(shader_program);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vert_count);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
