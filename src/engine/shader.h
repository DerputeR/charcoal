#pragma once
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace Charcoal {
class Shader {
    GLuint id;
public:
    Shader() = delete;
    explicit Shader(GLuint id);
    ~Shader() noexcept;

    // move constructors
    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    // don't allow copying
    Shader(const Shader &other) = delete;
    Shader &operator=(const Shader &other) = delete;

    void use();
    void set_float(const char* uniform_name, float value);
    void set_int(const char *uniform_name, int value);
    void set_mat4(const char *uniform_name, const glm::mat4 &value);
    bool is_valid() const;
};

class ShaderLoader {
    static const char *type_string(GLenum type);
    static GLuint compile(GLenum type, const GLchar *source);

public:
    static Shader from_strings(
            const char *vert_shader_src, const char *frag_shader_src);
    static Shader from_files(
            const char *vert_shader_path, const char *frag_shader_path);

    static const char *DEFAULT_VERT_PATH;
    static const char *DEFAULT_FRAG_PATH;
};

}
