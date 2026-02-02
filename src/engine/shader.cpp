#include "shader.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_iostream.h>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace Charcoal::Shader {

const char *Loader::DEFAULT_VERT_PATH = "resources/shaders/basic.vert.glsl";
const char *Loader::DEFAULT_FRAG_PATH = "resources/shaders/basic.frag.glsl";

const char *Loader::type_string(GLenum type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "vertex shader";
        case GL_FRAGMENT_SHADER:
            return "fragment shader";
        default:
            return "unknown shader type";
    }
}

GLuint Loader::compile(GLenum type, const GLchar *src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src,
            NULL); // returns a non-zero reference ID for the shader
    glCompileShader(id);

    // check if compilation had any errors
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLsizei msgLen;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetShaderInfoLog(id, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to compile %s: %s",
                type_string(type), &msg[0]);

        // cleanup failed shader compile
        glDeleteShader(id);

        return 0;
    }

    return id;
}

Program Loader::from_files(
        const char *vert_shader_path, const char *frag_shader_path) {
    char *vert_shader =
            static_cast<char *>(SDL_LoadFile(vert_shader_path, nullptr));
    char *frag_shader =
            static_cast<char *>(SDL_LoadFile(frag_shader_path, nullptr));
    Program program = from_strings(vert_shader, frag_shader);
    SDL_free(vert_shader);
    SDL_free(frag_shader);
    return program;
}

Program Loader::from_strings(
        const char *vert_shader_src, const char *frag_shader_src) {
    static_assert(sizeof(char) == sizeof(GLchar));

    if (vert_shader_src == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot compile shader program "
                                             "with null vertex shader source.");
        return Program{0};
    }

    if (frag_shader_src == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Cannot compile shader program with "
                "null fragment shader source.");
        return Program{0};
    }

    GLuint vs = compile(GL_VERTEX_SHADER, vert_shader_src);
    if (vs == 0) {
        SDL_LogError(
                SDL_LOG_CATEGORY_ERROR, "Aborting shader program creation.");
        return Program{0};
    }

    GLuint fs = compile(GL_FRAGMENT_SHADER, frag_shader_src);
    if (fs == 0) {
        SDL_LogError(
                SDL_LOG_CATEGORY_ERROR, "Aborting shader program creation.");
        return Program{0};
    }

    GLuint program_id = glCreateProgram(); // non-zero id
    if (program_id == 0) {
        SDL_LogError(
                SDL_LOG_CATEGORY_ERROR, "Failed to create program object.");
        return Program{program_id};
    }
    glAttachShader(program_id, vs);
    glAttachShader(program_id, fs);

    glLinkProgram(program_id);

    // linking done, detach shader intermediates
    glDetachShader(program_id, vs);
    glDetachShader(program_id, fs);

    // delete shader intermediates once detached
    glDeleteShader(vs);
    glDeleteShader(fs);

    // check if linking had errors
    GLint success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint msgLen;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetProgramInfoLog(program_id, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Shader program linking failed: %s", &msg[0]);

        return Program{0};
    }

    glValidateProgram(program_id);

    // check if validation had errors
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint msgLen;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetProgramInfoLog(program_id, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                "Shader program validation failed: %s", &msg[0]);

        return Program{0};
    }

    return Program{program_id};
}

Program::Program(GLuint id) : id{id} {
}

Program::~Program() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

Program::Program(Program &&other) noexcept : id{other.id} {
    other.id = 0;
}

Program &Program::operator=(Program &&other) noexcept {
    if (this != &other) {
        this->id = other.id;
        other.id = 0;
    }
    return *this;
}

void Program::use() {
    assert(is_valid());
    glUseProgram(id);
}

void Program::setFloat(const char *uniform_name, float value) {
    static_assert(sizeof(char) == sizeof(GLchar));
    static_assert(sizeof(float) == sizeof(GLfloat));
    GLint loc = glGetUniformLocation(id, uniform_name);
    if (loc > -1) {
        glUniform1f(loc, value);
    } else {
        // todo: error message handling
    }
}

void Program::setInt(const char *uniform_name, int value) {
    static_assert(sizeof(char) == sizeof(GLchar));
    static_assert(sizeof(int) == sizeof(GLint));
    GLint loc = glGetUniformLocation(id, uniform_name);
    if (loc > -1) {
        glUniform1i(loc, value);
    } else {
        // todo: error message handling
    }
}

void Program::setMat4(const char *uniform_name, const glm::mat4 &value) {
    static_assert(sizeof(char) == sizeof(GLchar));
    static_assert(sizeof(int) == sizeof(GLint));
    GLint loc = glGetUniformLocation(id, uniform_name);
    if (loc > -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    } else {
        // todo: error message handling
    }
}

bool Program::is_valid() const {
    return id != 0;
}

}