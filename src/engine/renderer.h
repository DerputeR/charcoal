#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

namespace Charcoal {

/**
 * @class Vertex
 * @brief Defines a Vertex, the base data structure to be submitted to shaders.
 *
 */
struct Vertex {
    glm::vec3 position;

    Vertex();
    Vertex(const glm::vec3 &position);
};

/**
 * @class Mesh
 * @brief Defines a Mesh, which is a collection of triangles defined by their
 * vertices and the indices which dictate their usage.
 *
 */
struct Mesh {
    std::vector<Vertex> verts;
    std::vector<int> indices;
};

/**
 * @class Renderer
 * @brief A primitive renderer using OpenGL.
 *        This renderer only renders triangles using an EBO bound to a VAO.
 *
 */
class Renderer {
public:
    /**
     * @brief Enum class containing the types of errors the Renderer can have.
     */
    enum class Error {
        none,
        invalid_program,
        invalid_vbo,
        invalid_vao,
        invalid_ebo,
    };

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint shader_program;
    GLuint index_count;
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
    void submit_mesh(const Mesh &mesh);
    void render();
    Error get_error() const;
    void clear_error();
    std::string get_error_msg() const;
};
} // namespace Charcoal
