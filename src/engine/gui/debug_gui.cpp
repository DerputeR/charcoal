#include "debug_gui.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL_log.h>

namespace Charcoal::Gui {
void DebugGui::draw(AppState* app_state) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // draw stuff
    draw_fps(&app_state->config.show_fps);


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugGui::draw_fps(bool *show) {
    ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoDecoration;
    if (ImGui::Begin("debug_fps", show, flags)) {
        ImGuiIO &io = ImGui::GetIO();

        // Basic info
        ImGui::Text("FPS: %.0f (avg %.3f ms/frame)", io.Framerate,
                1000.0f / io.Framerate);
    }
    ImGui::End();
}
} // namespace Charcoal::Gui
