#include "../engine/renderer.h"
#include "../engine/scene.h"

namespace Charcoal {
class TriangleScene : public Scene {
    std::vector<Vertex> verts{{{-0.5f, -0.5f, 0.0f}}, {{0.5f, -0.5f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}}};

public:
    void update(float delta_time);
    const std::vector<Vertex> &get_verts() const;

    ~TriangleScene();
};
} // namespace Charcoal
