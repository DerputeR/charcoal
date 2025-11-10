#pragma once
#include "time.h"

namespace Charcoal {
class Scene {
public:
    virtual void update(const Time &time) = 0;
    virtual ~Scene() = 0;
};
} // namespace Charcoal
