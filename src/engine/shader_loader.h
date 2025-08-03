#pragma once
#include <glad/glad.h>
#include <string>

class ShaderLoader {
    static const char *type_string(GLenum type);
    static GLuint compile(GLenum type, const GLchar *source);

public:
    static GLuint create_program(
            const GLchar *vert_shader_src, const GLchar *frag_shader_src);

    static const GLchar *DEFAULT_VERT_SRC;
    static const GLchar *DEFAULT_FRAG_SRC;
};
