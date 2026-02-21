#pragma once
#include "config.h"
#include "glad/glad.h"
#include "gui/debug_gui.h"
#include "scene.h"
#include "time.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include <memory>

namespace Charcoal {
struct AppState {
    Time time;
    Config config;
    Gui::DebugGui debug_gui;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<GpuMesh> gpu_mesh;
    std::vector<GpuTexture> gpu_texture;
    std::unique_ptr<Shader::Program> shader;
};
} // namespace Charcoal
