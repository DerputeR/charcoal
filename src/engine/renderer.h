#pragma once
#include <cstddef>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

namespace Charcoal {
struct Vertex {
    glm::vec3 position;

    Vertex();
    Vertex(const glm::vec3 &position);
};

class Renderer {
    GLuint vbo;
    GLuint vao;
    GLuint shader_program;
    GLuint vert_count;

public:
    Renderer();
    Renderer(GLuint shader_program);
    ~Renderer();
    void set_shader_program(GLuint program);
    void submit_verts(const std::vector<Vertex> &verts);
    void render();
};
} // namespace Charcoal
