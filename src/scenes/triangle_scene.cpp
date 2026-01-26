#include "triangle_scene.h"
#include <cmath>
#include <glm/gtc/quaternion.hpp>

namespace Charcoal {
TriangleScene::TriangleScene() {
}

TriangleScene::~TriangleScene() {
}

void TriangleScene::update(const Time &time) {
    // TODO
    float time_value = time.ns_to_f32(time.get_total_time());
    translation.x = std::sin(time_value) / 2.0f;
    rotation = glm::qua(glm::vec3{0.0f, 0.0f, time_value * glm::pi<float>()});
    t_dirty = true;
}

glm::mat4 TriangleScene::get_local_transform_matrix() {
    if (t_dirty) {
        local_transform_matrix = glm::translate(glm::mat4{1.0f}, translation) *
                                 glm::mat4_cast(rotation) * glm::mat4{scale};
        t_dirty = false;
    }
    return local_transform_matrix;
}

const std::vector<Mesh> &TriangleScene::get_meshes() const {
    return meshes;
}
} // namespace Charcoal
