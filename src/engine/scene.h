#pragma once

namespace Charcoal {
class Scene {
public:
    virtual void update(float delta_time) = 0;
    virtual ~Scene() = 0;
};
} // namespace Charcoal
