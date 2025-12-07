#pragma once
#include "mesh.h"
#include "time.h"
#include <vector>

namespace Charcoal {
class Scene {
public:
    virtual void update(const Time &time) = 0;
    const virtual std::vector<Mesh> &get_meshes() const = 0;
    virtual ~Scene() = 0;
};
} // namespace Charcoal
