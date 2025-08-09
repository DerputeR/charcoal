#include "triangle_scene.h"

namespace Charcoal {
TriangleScene::~TriangleScene() {
}

void TriangleScene::update(float delta_time) {
    // TODO
}

const std::vector<Vertex> &TriangleScene::get_verts() const {
    return verts;
}
} // namespace Charcoal
