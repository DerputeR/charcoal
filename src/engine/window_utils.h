#pragma once

#include <SDL3/SDL_events.h>
#include "SDL3/SDL_video.h"
#include "app_state.h"

namespace Charcoal {
void handle_window_rescale(SDL_Window *window, AppState *app_state, int newX, int newY);
}

