#pragma once
#include "config.h"
#include "time.h"
#include "SDL3/SDL_video.h"
#include "glad/glad.h"

namespace Charcoal {
struct AppState {
    Time time;
    Config config;
};
} // namespace Charcoal
