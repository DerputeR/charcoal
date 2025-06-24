#include "shader_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <errno.h>
#include <SDL3/SDL_log.h>
#include <vector>

const GLchar* ShaderLoader::DEFAULT_VERT_SRC = R"(
#version 330 core
layout (location = 0) in vec3 pos;

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
)";

const GLchar* ShaderLoader::DEFAULT_FRAG_SRC = R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
} 
)";

const char* ShaderLoader::type_string(GLenum type) {
    switch (type) {
    case GL_VERTEX_SHADER:
        return "vertex shader";
    case GL_FRAGMENT_SHADER:
        return "fragment shader";
    default:
        return "unknown shader type";
    }
}

GLuint ShaderLoader::compile(GLenum type, const GLchar* src) {
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL); // returns a non-zero reference ID for the shader
    glCompileShader(id);

    // check if compilation had any errors
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLsizei msgLen;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetShaderInfoLog(id, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to compile %s: %s", type_string(type), &msg[0]);

        // cleanup failed shader compile
        glDeleteShader(id);

        return 0;
    }

    return id;
}

GLuint ShaderLoader::create_program(const GLchar* vert_shader_src, const GLchar* frag_shader_src) {
    if (vert_shader_src == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot compile shader program with null vertex shader source.");
        return 0;
    }

    if (frag_shader_src == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot compile shader program with null fragment shader source.");
        return 0;
    }

    GLuint program = glCreateProgram(); // non-zero id

    GLuint vs = compile(GL_VERTEX_SHADER, vert_shader_src);
    if (!vs) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Aborting shader program creation.");
        return 0;
    }

    GLuint fs = compile(GL_FRAGMENT_SHADER, frag_shader_src);
    if (!fs) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Aborting shader program creation.");
        return 0;
    }


    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    // linking done, detach shader intermediates
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    // delete shader intermediates once detached
    glDeleteShader(vs);
    glDeleteShader(fs);


    // check if linking had errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint msgLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetProgramInfoLog(program, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Shader program linking failed: %s", &msg[0]);

        return 0;
    }

    glValidateProgram(program);

    // check if validation had errors
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint msgLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgLen);
        std::vector<GLchar> msg(msgLen, 0);

        glGetProgramInfoLog(program, msgLen, &msgLen, &msg[0]);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Shader program validation failed: %s", &msg[0]);

        return 0;
    }


    return program;
}