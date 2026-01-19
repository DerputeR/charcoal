#include "triangle_scene.h"

namespace Charcoal {
TriangleScene::TriangleScene() {
}

TriangleScene::~TriangleScene() {
}

void TriangleScene::update(const Time &time) {
    // TODO
}

const std::vector<Mesh> &TriangleScene::get_meshes() const {
    return meshes;
}
} // namespace Charcoal
