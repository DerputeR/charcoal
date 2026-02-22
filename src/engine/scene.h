#pragma once

#include "time.h"
#include "vertex.h"
#include "mesh.h"
#include <glad/glad.h>
#include <glm/ext/quaternion_float.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Charcoal {
class Scene {
    // xyz rgb uv
    std::vector<Vertex> verts{
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}};
    std::vector<int> indices{0, 1, 2, 2, 1, 3};
    std::vector<Mesh> meshes = {{verts, indices}};

    // object transforms
    glm::mat4 local_transform_matrix{1.0f};
    glm::vec3 translation{0.0f, 0.0f, 0.0f};
    glm::quat rotation{};
    float scale = 1.0f;
    bool t_dirty = false;

public:
    void update(const Time &time);
    const std::vector<Mesh> &get_meshes() const;

    glm::mat4 get_local_transform_matrix();

    Scene();
    ~Scene();
};
} // namespace Charcoal
