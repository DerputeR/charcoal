#pragma once
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace Charcoal::Shader {
class Program {
    GLuint id;
public:
    Program() = delete;
    explicit Program(GLuint id);
    ~Program() noexcept;

    // move constructors
    Program(Program &&other) noexcept;
    Program &operator=(Program &&other) noexcept;

    // don't allow copying
    Program(const Program &other) = delete;
    Program &operator=(const Program &other) = delete;

    void use();
    void set_float(const char* uniform_name, float value);
    void set_int(const char *uniform_name, int value);
    void set_mat4(const char *uniform_name, const glm::mat4 &value);
    bool is_valid() const;
};

class Loader {
    static const char *type_string(GLenum type);
    static GLuint compile(GLenum type, const GLchar *source);

public:
    static Program from_strings(
            const char *vert_shader_src, const char *frag_shader_src);
    static Program from_files(
            const char *vert_shader_path, const char *frag_shader_path);

    static const char *DEFAULT_VERT_PATH;
    static const char *DEFAULT_FRAG_PATH;
};

}
