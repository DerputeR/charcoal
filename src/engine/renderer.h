#pragma once
#include "app_state.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
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
    glm::uint32 rgb;
    glm::vec2 uv;

    Vertex();
    Vertex(const glm::vec3 &position);
    Vertex(const glm::vec3 &position, const glm::vec3 &rgb);
    Vertex(const glm::vec3 &position, glm::uint32 rgb);
    Vertex(const glm::vec3 &position, const glm::vec3 &rgb,
            const glm::vec2 &uv);
    Vertex(const glm::vec3 &position, glm::uint32 rgb, const glm::vec2 &uv);

    /**
     * @brief Helper class to set vertex color using integer RGB representation
     * @param r Clamped to [0, 255]
     * @param g Clamped to [0, 255]
     * @param b Clamped to [0, 255]
     */
    void set_rgb(int r, int g, int b);

    /**
     * @brief Helper class to pack 8-bit RGB components into a single 32-bit
     * unsigned int
     *
     * @param r Clamped to [0, 255]
     * @param g Clamped to [0, 255]
     * @param b Clamped to [0, 255]
     */
    static glm::uint32 pack_rgb24_to_uint32(int r, int g, int b);

    static glm::uint32 pack_normalized_rgb24_to_uint32(
            float r, float g, float b);
    static glm::uint32 pack_normalized_rgb24_to_uint32(const glm::vec3 rgb);
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
        opengl_error,
    };

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint shader_program;
    GLuint index_count;
    // TODO: map attributes dynamically, lookup per shader
    GLint position_index;
    GLint rgb_index;
    GLint uv_index;
    // TODO: figure out how to do textures properly
    GLuint texture;
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
    void render(AppState *app_state);
    Error get_error() const;
    void clear_error();
    std::string get_error_msg() const;
};
} // namespace Charcoal
