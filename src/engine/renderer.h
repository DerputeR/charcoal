#pragma once
#include "app_state.h"
#include "mesh.h"
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

namespace Charcoal {

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
    GLuint texture[2];
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
