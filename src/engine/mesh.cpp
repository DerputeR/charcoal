#include "mesh.h"
#include <SDL3/SDL_log.h>
#include <cassert>

namespace Charcoal {

GpuMesh::GpuMesh() :
        vbo{0}, vao{0}, ebo{0},
        element_count{0}, error{Error::none} {
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
}

void GpuMesh::init_attribute_layout() {
    // attrib index, attrib element count, attrib element type,
    // normalized, size of vertex (stride), attrib offset within vertex
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex),
            reinterpret_cast<GLvoid *>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(ATTRIB_POSITION);

    glVertexAttribIPointer(ATTRIB_COLOR, 1, GL_UNSIGNED_INT, sizeof(Vertex),
            reinterpret_cast<GLvoid *>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(ATTRIB_COLOR);

    glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<GLvoid *>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(ATTRIB_UV);
}

GpuMesh::GpuMesh(GpuMesh &&other) noexcept :
        vbo{other.vbo}, vao{other.vao}, ebo{other.ebo},
        element_count{other.element_count}, error{Error::none} {
    other.vbo = 0;
    other.vao = 0;
    other.ebo = 0;
    other.element_count = 0;
    other.error = Error::destroyed;
}

GpuMesh &GpuMesh::operator=(GpuMesh &&other) noexcept {
    if (this != &other) {
        this->vbo = other.vbo;
        this->vao = other.vao;
        this->ebo = other.ebo;
        this->element_count = other.element_count;
        other.vbo = 0;
        other.vao = 0;
        other.ebo = 0;
        other.element_count = 0;
    }
    return *this;
}

GpuMesh::~GpuMesh() noexcept {
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao != 0) {
        glDeleteBuffers(1, &vao);    
    }
    if (ebo != 0) {
        glDeleteBuffers(1, &ebo);
    }
}

void GpuMesh::upload(const Mesh &mesh) {
    bind_vao();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.verts.size() * sizeof(Vertex),
            mesh.verts.data(), GL_STATIC_DRAW);

    // check if the data was uploaded correctly
    GLint buf_size = 0;
    std::size_t expected_size = mesh.verts.size() * sizeof(Vertex);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
    if (static_cast<std::size_t>(buf_size) != expected_size) {
        SDL_LogError(SDL_LOG_CATEGORY_GPU,
                "VBO buffer size %d was expected to be size %zu", buf_size,
                expected_size);
        error = Error::invalid_vbo;
        return;
    }

    // copy the indices to the ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(int),
            mesh.indices.data(), GL_STATIC_DRAW);

    // check if the data was uploaded correctly
    buf_size = 0;
    expected_size = mesh.indices.size() * sizeof(int);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
    if (static_cast<std::size_t>(buf_size) != expected_size) {
        SDL_LogError(SDL_LOG_CATEGORY_GPU,
                "EBO buffer size %d was expected to be size %zu", buf_size,
                expected_size);
        error = Error::invalid_ebo;
        return;
    } else {
        element_count = mesh.indices.size();
    }
    init_attribute_layout();
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        error = Error::unknown;
        SDL_LogError(SDL_LOG_CATEGORY_GPU,
                "OpenGL error while uploading to GpuMesh: %u", err);
        return;
    }
    error = Error::none;
}

void GpuMesh::bind_vao() {
    assert(is_valid());
    glBindVertexArray(vao);
}

GpuMesh::Error GpuMesh::get_error() const {
    return error;
}

bool GpuMesh::is_valid() const {
    return error == Error::none;
}

GLuint GpuMesh::get_element_count() const {
    return element_count;
}

}