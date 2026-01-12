#pragma once
#include <imgui.h>

namespace Charcoal {
struct AppState;
}

namespace Charcoal::Gui {
class DebugGui {
    void draw_fps(bool *show);
public:
    void draw(AppState* app_state);
    static ImGuiStyle default_style();
};
} // namespace Charcoal::Gui
