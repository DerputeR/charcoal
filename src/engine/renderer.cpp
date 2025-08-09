#include "renderer.h"
#include "shader_loader.h"
#include <SDL3/SDL_log.h>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) : position{position} {
}

Renderer::Renderer() :
        Renderer(ShaderLoader::create_program(ShaderLoader::DEFAULT_VERT_SRC,
                ShaderLoader::DEFAULT_FRAG_SRC)) {
}

Renderer::Renderer(GLuint shader_program) : shader_program{shader_program} {
    if (shader_program == 0) {
        SDL_LogCritical(
                SDL_LOG_CATEGORY_VIDEO, "Invalid shader program loaded");
    }

    glGenBuffers(1, &vbo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // glVertexAttribPointer(
    //         0, 1, GL_FLOAT_VEC3, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<GLvoid *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
}

void Renderer::set_shader_program(GLuint program) {
    shader_program = program;
}

void Renderer::submit_verts(const std::vector<Vertex> &verts) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(),
            GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vert_count = verts.size();
}

void Renderer::render() {
    glUseProgram(shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vert_count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &vbo);
}
} // namespace Charcoal
