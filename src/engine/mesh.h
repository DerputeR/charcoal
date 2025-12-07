#pragma once
#include "vertex.h"
#include <vector>

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
} // namespace Charcoal
