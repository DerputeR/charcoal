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

#include "engine/app_state.h"
#include "engine/config.h"
#include "engine/gui/debug_gui.h"
#include "engine/renderer.h"
#include "engine/shader_loader.h"
#include "engine/time.h"
#include "engine/window_utils.h"

#include "scenes/triangle_scene.h"

#include "app_info.h"

static Charcoal::Gui::DebugGui debug_gui;
static std::unique_ptr<Charcoal::Renderer> renderer;
static SDL_Window *window;
static SDL_GLContext gl_context;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    SDL_SetAppMetadata(APP_FULL_NAME, APP_VERSION, APP_PACKAGE);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_WARN);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ASSERT, "SDL failed to init: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Appstate init
    Charcoal::AppState *app_state = new Charcoal::AppState();
    app_state->scene = std::make_unique<Charcoal::TriangleScene>();
    *appstate = app_state;

    // Window init
    app_state->config.dpi_scaling = 1.0f;
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
                    "Unable to detect primary display's scale: %s\nDefaulting "
                    "to normal scaling",
                    SDL_GetError());
            main_scale = 1.0f;
        }
        app_state->config.dpi_scaling = main_scale;
    }

    window = SDL_CreateWindow(APP_WINDOW_TITLE,
            static_cast<int>(
                    static_cast<float>(app_state->config.resolution.x) *
                    app_state->config.dpi_scaling),
            static_cast<int>(
                    static_cast<float>(app_state->config.resolution.y) *
                    app_state->config.dpi_scaling),
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
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to create OpenGL context for window: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_GL_MakeCurrent(window, gl_context)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to set context as current for window: %s",
                SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (app_state->config.vsync_enabled) {
        if (app_state->config.vsync_adaptive) {
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
        app_state->time.set_fps_cap(app_state->config.fps_max);
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
    GLuint default_shader_program =
            Charcoal::Shader::ShaderLoader::create_program(
                    Charcoal::Shader::ShaderLoader::DEFAULT_VERT_SRC,
                    Charcoal::Shader::ShaderLoader::DEFAULT_FRAG_SRC);
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
            ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |=
            ImGuiConfigFlags_DockingEnable; // IF using Docking Branch
                                            // io.ConfigDpiScaleFonts = true; //
                                            // this is marked as EXPERIMENTAL
                                            // io.ConfigDpiScaleViewports =
                                            // true; // this is marked as
                                            // EXPERIMENTAL
    io.IniFilename =
            nullptr; // do not load from ini file. we can customize this later

    // Styling
    ImGui::StyleColorsDark(nullptr);

    // Styling
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(app_state->config.dpi_scaling);
    // available only in >= 1.92, experimental api
    // style.FontScaleDpi = app_state->config.dpi_scaling;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(
            "#version 330 core"); // glad was configured to use 3.3 core

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    Charcoal::AppState *app_state =
            reinterpret_cast<Charcoal::AppState *>(appstate);

    // compute previous frame time
    app_state->time.update(SDL_GetTicksNS(), true);

    // manual framecap when vsync is off
    auto min_frame_time = app_state->time.get_min_frame_time_ns();
    auto time_ns_delta = app_state->time.get_delta_ns();
    if ((!app_state->config.vsync_enabled ||
                (app_state->config.vsync_enabled &&
                        app_state->config.vsync_adaptive)) &&
            min_frame_time > 0) {
        if (time_ns_delta < min_frame_time) {
            SDL_DelayPrecise(min_frame_time - time_ns_delta);
            app_state->time.update(SDL_GetTicksNS(), false);
        }
    }

    // update the scene
    // TODO:: separate scene and renderer components
    // Ideally we'd like to have the scene loaded at runtime dynamically
    app_state->scene->update(app_state->time);

    // clear the buffer
    glClearColor(app_state->config.clear_color.r,
            app_state->config.clear_color.g, app_state->config.clear_color.b,
            app_state->config.clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the scene
    // TODO: submit draw calls/update buffers if they changed before this step
    if (app_state->time.get_frame_count() == 1) {
        Charcoal::TriangleScene *scene =
                reinterpret_cast<Charcoal::TriangleScene *>(
                        app_state->scene.get());
        renderer->submit_mesh(scene->get_meshes()[0]);
        if (renderer->get_error() != Charcoal::Renderer::Error::none) {
            SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "%s",
                    renderer->get_error_msg().c_str());
            return SDL_APP_FAILURE;
        }
    }
    renderer->render(app_state);

    // draw the GUI
    debug_gui.draw(app_state);

    // display the render
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    Charcoal::AppState *app_state =
            reinterpret_cast<Charcoal::AppState *>(appstate);
    ImGui_ImplSDL3_ProcessEvent(event);
    switch (event->type) {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
        }
        // case SDL_EVENT_WINDOW_RESIZED: {
        //     SDL_WindowEvent *window_event =
        //             reinterpret_cast<SDL_WindowEvent *>(event);
        //     SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "normal resize event: %d x
        //     %d", window_event->data1, window_event->data2); break;
        // }
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
            SDL_WindowEvent *window_event =
                    reinterpret_cast<SDL_WindowEvent *>(event);
            SDL_Window *window = SDL_GetWindowFromID(window_event->windowID);
            Charcoal::handle_window_rescale(window, app_state,
                    window_event->data1, window_event->data2);
            break;
        }
            // TODO: rescale on display change
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    Charcoal::AppState *app_state =
            reinterpret_cast<Charcoal::AppState *>(appstate);
    delete app_state;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
}
