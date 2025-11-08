#pragma once
#include "engine/config.h"
#include "engine/time.h"

namespace Charcoal {
struct AppState {
    Time time;
    Config config;
};
} // namespace Charcoal
