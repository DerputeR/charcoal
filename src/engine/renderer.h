#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

namespace Charcoal {
struct Vertex {
    glm::vec3 position;

    Vertex();
    Vertex(const glm::vec3 &position);
};

class Renderer {
public:
    enum class Error {
        none,
        invalid_program,
        invalid_vbo,
        invalid_vao,
    };

private:
    GLuint vbo;
    GLuint vao;
    GLuint shader_program;
    GLuint vert_count;
    // TODO: map attributes dynamically, lookup per shader
    GLint position_index;
    Error error;
    std::string error_msg;
    void set_error(Error e, const std::string &msg);
    void load_attribute_indices();

public:
    Renderer();
    Renderer(GLuint shader_program);
    ~Renderer();
    void set_shader_program(GLuint program);
    void submit_verts(const std::vector<Vertex> &verts);
    void render();
    Error get_error() const;
    void clear_error();
    std::string get_error_msg() const;
};
} // namespace Charcoal
