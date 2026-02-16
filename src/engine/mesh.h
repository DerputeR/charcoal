#pragma once
#include "vertex.h"
#include <vector>
#include <glad/glad.h>

namespace Charcoal {
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
 * @class GpuMesh
 * @brief Defines the data needed to bind a mesh to the GPU. Includes the VAO,
 * EBO, and VBO.
 */
class GpuMesh {
public:
    enum class Error {
        none,
        invalid_vbo,
        invalid_vao,
        invalid_ebo,
        destroyed,
        unknown
    };

private:
    GLuint vbo;
    GLuint ebo;
    GLuint vao;
    GLuint element_count;
    Error error;

    void init_attribute_layout();

    static constexpr int ATTRIB_POSITION = 0;
    static constexpr int ATTRIB_COLOR = 1;
    static constexpr int ATTRIB_UV = 2;

public:    
    explicit GpuMesh();

    // move constructors
    GpuMesh(GpuMesh &&other) noexcept;
    GpuMesh &operator=(GpuMesh &&other) noexcept;

    // don't allow copying
    GpuMesh(const GpuMesh &other) = delete;
    GpuMesh &operator=(const GpuMesh &other) = delete;


    ~GpuMesh() noexcept;
    /**
     * @brief Upload the data from the given Mesh to the GPU.
     * Note that this currently overwrites the GPU buffer data, not append.
     * If the operation fails, the function will exit early, and an error
     * will be recorded and logged. Use GpuMesh::get_error() to view the
     * error type.
     * @param mesh The mesh from the CPU to upload
     */
    void upload(const Mesh &mesh);

    /**
     * @brief Binds the GpuMesh's VAO to the current OpenGL context
     */
    void bind();

    /**
     * @brief Checks if the GpuMesh is in a valid state.
     * @return True only if the last GpuMesh operation did not result in an error.
     */
    bool is_valid() const;

    /**
     * @brief Returns the first error type encountered during a GpuMesh operation.
     * @return One of GpuMesh::Error
     */
    Error get_error() const;

    /**
     * @brief Returns the number of elements the EBO has. 
     * @return The number of elements in the EBO
     */
    GLuint get_element_count() const;
};

} // namespace Charcoal
