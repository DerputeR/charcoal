#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <glad/glad.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengl.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "engine/config.h"
#include "engine/renderer.h"
#include "engine/shader_loader.h"
#include "engine/time.h"

#include "scenes/triangle_scene.h"

#include "app_info.h"

static SDL_Window *window;
static SDL_GLContext glCtx;
static float dpi_scaling = 1.0f;

static Charcoal::Config config;
static Charcoal::Time engine_time;
static Charcoal::TriangleScene
        triangle_scene; // todo: replace with proper scene loading system
static std::unique_ptr<Charcoal::Renderer> renderer;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_SetAppMetadata(APP_FULL_NAME, APP_VERSION, APP_PACKAGE);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ASSERT, "SDL failed to init: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Window init
    float dpi_scaling = 1.0f;
    SDL_DisplayID primary_display = SDL_GetPrimaryDisplay();
    if (primary_display == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                "Unable to detect primary display: %s\nDefaulting to normal "
                "DPI scaling",
                SDL_GetError());
    } else {
        float main_scale = SDL_GetDisplayContentScale(primary_display);
        if (main_scale == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                    "Unable to detect priamry display's scale: %s\nDefaulting "
                    "to normal scaling",
                    SDL_GetError());
            main_scale = 1.0f;
        }
    }

    window = SDL_CreateWindow(APP_WINDOW_TITLE,
            static_cast<int>(
                    static_cast<float>(config.resolution.x) * dpi_scaling),
            static_cast<int>(
                    static_cast<float>(config.resolution.y) * dpi_scaling),
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
                    SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create window: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_SetWindowPosition(
                window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                "Failed to set window position: %s", SDL_GetError());
    }

    // OpenGL render context
    glCtx = SDL_GL_CreateContext(window);
    if (glCtx == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to create OpenGL context for window: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_GL_MakeCurrent(window, glCtx)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to set context as current for window: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (config.vsync_enabled) {
        if (config.vsync_adaptive) {
            if (!SDL_GL_SetSwapInterval(-1)) {
                SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                        "Failed to enable adaptive vsync, retrying with normal "
                        "vsync: %s",
                        SDL_GetError());
                if (!SDL_GL_SetSwapInterval(1)) {
                    SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                            "Failed to enable vsync: %s", SDL_GetError());
                }
            }
        } else if (!SDL_GL_SetSwapInterval(1)) {
            SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to enable vsync: %s",
                    SDL_GetError());
        }
    } else if (!SDL_GL_SetSwapInterval(0)) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to disable vsync: %s",
                SDL_GetError());
    } else {
        engine_time.set_fps_cap(config.fps_max);
    }

    // GLAD init
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to initialize GLAD");
        return SDL_APP_FAILURE;
    }

    // Initialize default shader as a check
    // TODO: do this in the renderer. maybe the renderer should be a simple
    // object that we initialize in steps prior to use? seems like an
    // antipattern but idk
    GLuint default_shader_program = ShaderLoader::create_program(
            ShaderLoader::DEFAULT_VERT_SRC, ShaderLoader::DEFAULT_FRAG_SRC);
    if (default_shader_program == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to create default shader program");
        return SDL_APP_FAILURE;
    }
    // using a unique_ptr for now but might go back on this
    renderer = std::make_unique<Charcoal::Renderer>(default_shader_program);
    if (renderer->get_error() != Charcoal::Renderer::Error::none) {
        SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "%s",
                renderer->get_error_msg().c_str());
        return SDL_APP_FAILURE;
    }

    // Dear ImGUI init
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr) {
        SDL_LogCritical(
                SDL_LOG_CATEGORY_VIDEO, "Failed to create ImGui Context");
        return SDL_APP_FAILURE;
    }
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch
    // io.ConfigDpiScaleFonts = true; // this is marked as EXPERIMENTAL
    // io.ConfigDpiScaleViewports = true; // this is marked as EXPERIMENTAL
    io.IniFilename =
            nullptr; // do not load from ini file. we can customize this later

    // Styling
    ImGui::StyleColorsDark(nullptr);

    // Styling
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(dpi_scaling);
    style.FontScaleDpi = dpi_scaling;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glCtx);
    ImGui_ImplOpenGL3_Init(
            "#version 330 core"); // glad was configured to use 3.3 core

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    static bool demo_window_shown = true;

    // compute previous frame time
    engine_time.update(SDL_GetTicksNS(), true);

    // manual framecap when vsync is off
    auto min_frame_time = engine_time.get_min_frame_time_ns();
    auto time_ns_delta = engine_time.get_delta_ns();
    if ((!config.vsync_enabled ||
                (config.vsync_enabled && config.vsync_adaptive)) &&
            min_frame_time > 0) {
        if (time_ns_delta < min_frame_time) {
            SDL_DelayPrecise(min_frame_time - time_ns_delta);
            engine_time.update(SDL_GetTicksNS(), false);
        }
    }

    // update the scene
    // TODO:: separate scene and renderer components
    // Ideally we'd like to have the scene loaded at runtime dynamically
    triangle_scene.update(engine_time.get_delta_ns());

    // clear the buffer
    glClearColor(config.clear_color.r, config.clear_color.g,
            config.clear_color.b, config.clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the scene
    // TODO: submit draw calls/update buffers if they changed before this step
    if (engine_time.get_frame_count() == 1) {
        renderer->submit_mesh(triangle_scene.get_meshes()[0]);
        if (renderer->get_error() != Charcoal::Renderer::Error::none) {
            SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "%s",
                    renderer->get_error_msg().c_str());
            return SDL_APP_FAILURE;
        }
    }
    renderer->render();

    // draw the GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(&demo_window_shown);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // display the render
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO,
                "Window scaling change detected. Automatic scaling update is "
                "not yet implemented.");
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
}
