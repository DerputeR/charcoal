#pragma once
#include "../app_state.h"

namespace Charcoal::Gui {
class DebugGui {
    void draw_fps(bool *show);
public:
    void draw(AppState* app_state);
};
} // namespace Charcoal::Gui
