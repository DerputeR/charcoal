#include "triangle_scene.h"

namespace Charcoal {
TriangleScene::~TriangleScene() {
}

void TriangleScene::update(float delta_time) {
    // TODO
}

const std::vector<Mesh> &TriangleScene::get_meshes() const {
    return meshes;
}
} // namespace Charcoal
