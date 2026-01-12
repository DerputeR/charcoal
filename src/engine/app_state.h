#pragma once
#include "config.h"
#include "glad/glad.h"
#include "gui/debug_gui.h"
#include "scene.h"
#include "time.h"
#include <memory>

namespace Charcoal {
struct AppState {
    Time time;
    Config config;
    Gui::DebugGui debug_gui;
    std::unique_ptr<Scene> scene;
};
} // namespace Charcoal
