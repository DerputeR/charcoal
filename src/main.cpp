#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengl.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "engine/time.h"
#include "engine/shader_loader.h"

#include "app_info.h"

static SDL_Window* window;
static SDL_GLContext glCtx;

static int fps_max = 400; // TODO: move to config

static bool vsync_enabled = false;
static bool vsync_adaptive = true;
static Charcoal::Time engine_time;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    SDL_SetAppMetadata(APP_FULL_NAME, APP_VERSION, APP_PACKAGE);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_DEBUG);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ASSERT, "SDL failed to init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Window init
    window = SDL_CreateWindow(APP_WINDOW_TITLE, 1280, 720,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // OpenGL render context
    glCtx = SDL_GL_CreateContext(window);
    if (glCtx == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create OpenGL context for window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_GL_MakeCurrent(window, glCtx)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to set context as current for window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (vsync_enabled) {
        if (vsync_adaptive) {
            if (!SDL_GL_SetSwapInterval(-1)) {
                SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to enable adaptive vsync, retrying with normal vsync: %s", SDL_GetError());
                if (!SDL_GL_SetSwapInterval(1)) {
                    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to enable vsync: %s", SDL_GetError());
                }
            }
        } else if (!SDL_GL_SetSwapInterval(1)) {
            SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to enable vsync: %s", SDL_GetError());
        }
    } else if (!SDL_GL_SetSwapInterval(0)) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Failed to disable vsync: %s", SDL_GetError());
    } else {
        engine_time.set_fps_cap(fps_max);
    }


    // GLAD init
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to initialize GLAD");
        return SDL_APP_FAILURE;
    }

    // Test shader loader
    GLuint default_program = ShaderLoader::create_program(ShaderLoader::DEFAULT_VERT_SRC, ShaderLoader::DEFAULT_FRAG_SRC);
    if (default_program == 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create default shader program");
        return SDL_APP_FAILURE;
    }

    // Dear ImGUI init
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create ImGui Context");
        return SDL_APP_FAILURE;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    io.IniFilename = nullptr; // do not load from ini file. we can customize this later

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glCtx);
    ImGui_ImplOpenGL3_Init("#version 330 core"); // glad was configured to use 3.3 core

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    static bool demo_window_shown = true;

    // compute previous frame time
    engine_time.update(SDL_GetTicksNS(), true);

    // manual framecap when vsync is off
    auto min_frame_time = engine_time.get_min_frame_time_ns();
    auto time_ns_delta = engine_time.get_delta_ns();
    if ((!vsync_enabled || (vsync_enabled && vsync_adaptive)) && min_frame_time > 0) {
        if (time_ns_delta < min_frame_time) {
            SDL_DelayPrecise(min_frame_time - time_ns_delta);
            engine_time.update(SDL_GetTicksNS(), false);
        }
    }

    // update the scene
    // TODO

    // clear the buffer
    glClearColor(32.0f/255.0f, 32.0f/255.0f, 32.0f/255.0f, SDL_ALPHA_OPAQUE_FLOAT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the scene
    // TODO

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

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    ImGui_ImplSDL3_ProcessEvent(event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
}
