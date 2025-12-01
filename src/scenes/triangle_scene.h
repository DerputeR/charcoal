#include "../engine/renderer.h"
#include "../engine/scene.h"
#include "../engine/time.h"

namespace Charcoal {
class TriangleScene : public Scene {
    std::vector<Vertex> verts{
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f}}};
    std::vector<int> indices{2, 0, 1};
    std::vector<Mesh> meshes = {{verts, indices}};

public:
    void update(const Time &time);
    const std::vector<Mesh> &get_meshes() const;

    TriangleScene();
    ~TriangleScene();
};
} // namespace Charcoal
