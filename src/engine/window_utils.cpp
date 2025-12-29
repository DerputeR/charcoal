#include "window_utils.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>
#include <imgui.h>

namespace Charcoal {

// todo: handle scaling update detection separately
// todo: allow programmatic rescaling (i.e. via console command or settings
// menu) todo: allow changing window dpi scaling in-app
void handle_window_rescale(
        SDL_Window *window, AppState *app_state, int newX, int newY) {
    float new_scale = SDL_GetWindowDisplayScale(window);
    if (new_scale == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                "Unable to detect primary display's scale: "
                "%s\nDefaulting "
                "to normal scaling",
                SDL_GetError());
        new_scale = 1.0f;
    }

    float old_scaling = app_state->config.dpi_scaling;

    app_state->config.dpi_scaling = new_scale;
    app_state->config.resolution.x = newX / new_scale;
    app_state->config.resolution.y = newY / new_scale;

    glViewport(0, 0, newX, newY);

    // todo: this might be buggy
    if (new_scale != old_scaling) {
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(1.0f / old_scaling);
        style.ScaleAllSizes(new_scale);
        // available only in >= 1.92, experimental api
        // style.FontScaleDpi = new_scale;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_VIDEO,
            "Window size changed to %d x %d (%d x %d with DPI scale %.3f)",
            newX, newY, app_state->config.resolution.x,
            app_state->config.resolution.y, app_state->config.dpi_scaling);
}
} // namespace Charcoal
