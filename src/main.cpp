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
//#include "engine/renderer.h"
#include "engine/shader.h"
#include "engine/time.h"
#include "engine/window_utils.h"
#include "engine/scene.h"

#include "app_info.h"

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

    // Init appstate
    // we must ensure any OpenGL related state is initialized
    // *after* the render context is initialized
    Charcoal::AppState *app_state = new Charcoal::AppState();
    *appstate = app_state;

    // Init window
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

    // Init OpenGL render context
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

    // Configure vsync
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

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to initialize GLAD");
        return SDL_APP_FAILURE;
    }

    // Configure render pipeline
    // face must be front and back. mode can be fill or wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Init CPU to GPU bridge
    app_state->gpu_mesh = std::make_unique<Charcoal::GpuMesh>();

    // Init default shader
    app_state->shader = std::make_unique<Charcoal::Shader::Program>(
            Charcoal::Shader::Loader::from_files(
                    Charcoal::Shader::Loader::DEFAULT_VERT_PATH,
                    Charcoal::Shader::Loader::DEFAULT_FRAG_PATH));
    if (!app_state->shader->is_valid()) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
                "Failed to create default shader program");
        return SDL_APP_FAILURE;
    }

    // Init scene
    app_state->scene = std::make_unique<Charcoal::Scene>();

    // Init textures
    // TODO:

    // Upload mesh
    assert(app_state->scene->get_meshes().size() > 0);
    // todo: support merging meshes into a single big buffer
    app_state->gpu_mesh->upload(app_state->scene->get_meshes()[0]);
    if (!app_state->gpu_mesh->is_valid()) {
        return SDL_APP_FAILURE;    
    }

    // Init Dear ImGUI
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
    io.FontGlobalScale = app_state->config.dpi_scaling;
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
    app_state->scene->update(app_state->time);

    // clear the buffer
    glClearColor(app_state->config.clear_color.r,
            app_state->config.clear_color.g, app_state->config.clear_color.b,
            app_state->config.clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // compute per-frame values for uniforms later
    float time_value =
            app_state->time.ns_to_f32(app_state->time.get_total_time());
    glm::mat4 transform = app_state->scene->get_local_transform_matrix();
    float blend_amount = 0.5f + (std::sin(time_value * 2.0f) / 2.0);

    // bind shader + set uniforms
    app_state->shader->use();
    app_state->shader->set_mat4("transform", transform);
    app_state->shader->set_float("blend", blend_amount);

    // bind textures
    // TODO: need to do this part or glDrawElements just crashes immediately

    app_state->gpu_mesh->bind();
    glDrawElements(GL_TRIANGLES, app_state->gpu_mesh->get_element_count(),
            GL_UNSIGNED_INT, 0);

    // draw the GUI
    app_state->debug_gui.draw(app_state);

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
