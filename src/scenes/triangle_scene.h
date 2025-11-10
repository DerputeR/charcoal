#include "../engine/renderer.h"
#include "../engine/scene.h"

namespace Charcoal {
class TriangleScene : public Scene {
    std::vector<Vertex> verts{{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
    std::vector<int> indices{2, 0, 1};
    std::vector<Mesh> meshes = {{verts, indices}};

public:
    void update(float delta_time);
    const std::vector<Mesh> &get_meshes() const;

    ~TriangleScene();
};
} // namespace Charcoal
