#pragma once
#include "SDL3/SDL_pixels.h"
#include <glm/ext/vector_uint2.hpp>
namespace Charcoal {
// this is a stop-gap solution until I get around to implementing ConVars
struct Config {
    int fps_max = 400;
    bool vsync_enabled = false;
    bool vsync_adaptive = false;
    SDL_FColor clear_color{
            32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f};
    glm::uvec2 resolution{1280, 720};
};
} // namespace Charcoal
