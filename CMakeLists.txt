##########################################################
#                    PROJECT SETUP                       #
##########################################################

# TODO: reference https://github.com/Ravbug/sdl3-sample/blob/main/CMakeLists.txt
# Update to support Emscripten builds, Apple (need hardware/VM to test)
# Make labeling nicer

cmake_minimum_required(VERSION 3.31...4.0)

project(Charcoal VERSION 0.1.0
    DESCRIPTION "A terrible, inefficient, but custom-made game engine"
    LANGUAGES C CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Export compile commands for things like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# This ensures that the dynamic lib goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# Prevent installing to system directories
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")
endif()

# Platform-specific configurations
# Apple - This is untested!
if((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

# Windows
if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)
    endif()
endif()


##########################################################
#                      DEPENDENCIES                      #
##########################################################

set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL
    "If enabled, only fetch content the first time.
    Future runs will not attempt to redownload."
)
include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY  https://github.com/ocornut/imgui
    GIT_TAG         v1.92.4-docking
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY  https://github.com/g-truc/glm
    GIT_TAG         1.0.2
)

FetchContent_MakeAvailable(imgui glm)


# Linking SDL instructions are found in SDL/docs/README-cmake.md
# Create an option to switch between a system SDL lib and a vendored one
# If this variable exists in the cache, its value will not be overwritten
option(MYGAME_VENDORED "Use vendored libraries" OFF)
set(SDLIMAGE_VENDORED ${MYGAME_VENDORED})
# if(NOT DEFINED CMAKE_PREFIX_PATH)
#     set(CMAKE_PREFIX_PATH "/usr/local" CACHE PATH
#         "Custom prefix paths to search for libraries under")
# endif()

# Disable unused image formats - do this BEFORE getting the dependency
option(SDLIMAGE_ANI "Support loading ANI animations" OFF)
option(SDLIMAGE_AVIF "Support loading AVIF images" OFF)
option(SDLIMAGE_BMP "Support loading BMP images" OFF)
option(SDLIMAGE_GIF "Support loading GIF images" OFF)
option(SDLIMAGE_JPG "Support loading JPEG images" OFF)
option(SDLIMAGE_JXL "Support loading JXL images" OFF)
option(SDLIMAGE_LBM "Support loading LBM images" OFF)
option(SDLIMAGE_PCX "Support loading PCX images" OFF)
option(SDLIMAGE_PNG "Support loading PNG images" ON)
option(SDLIMAGE_PNM "Support loading PNM images" OFF)
option(SDLIMAGE_QOI "Support loading QOI images" OFF)
option(SDLIMAGE_SVG "Support loading SVG images" OFF)
option(SDLIMAGE_TGA "Support loading TGA images" OFF)
option(SDLIMAGE_TIF "Support loading TIFF images" OFF)
option(SDLIMAGE_WEBP "Support loading WEBP images" OFF)
option(SDLIMAGE_XCF "Support loading XCF images" OFF)
option(SDLIMAGE_XPM "Support loading XPM images" OFF)
option(SDLIMAGE_XV "Support loading XV images" OFF)


# Note: An older version of this CMakeLists.txt used GLFW as a dependency.
# This is no longer the case, but it's not a bad idea to specify library mode.
# Before, GLFW set the variable BUILD_SHARED_LIBS to false before we hit SDL,
# so we needed to manually enable SDL_SHARED before acquiring SDL
# option(SDL_SHARED "Build a SDL shared library (if available)" ON)
# option(SDL_STATIC "Build a SDL static library (if available)" OFF)

if(MYGAME_VENDORED)
    add_subdirectory("vendored/SDL" EXCLUDE_FROM_ALL)
    add_subdirectory("vendored/SDL_image" EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL
        GIT_TAG         release-3.2.24
        EXCLUDE_FROM_ALL
    )
    FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL_image
        GIT_TAG         release-3.2.4
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(SDL3)
    FetchContent_MakeAvailable(SDL3_image)
endif()

# Dear ImGUI doesn't have its own CMakeLists.txt so we'll have to declare
# which sources we're using so we can embed it directly into the executable
set(IMGUI_SOURCES "${imgui_SOURCE_DIR}/imgui.cpp"
                  "${imgui_SOURCE_DIR}/imgui_demo.cpp"
                  "${imgui_SOURCE_DIR}/imgui_draw.cpp"
                  "${imgui_SOURCE_DIR}/imgui_tables.cpp"
                  "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
                  "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

# We will also need to do so for the backend sources
set(IMGUI_BACKEND_SOURCES "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
                          "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

# This is recommended for debugging in VS
set(IMGUI_DEBUG "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natvis"
                "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter"
)


##########################################################
#                 SOURCE PRE-PROCESSING                  #
##########################################################

# App versioning/title
set(APP_PACKAGE "com.example.charcoal")
set(APP_FULL_NAME "Charcoal Engine")
set(APP_WINDOW_TITLE "Charcoal Engine")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app_info.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/app_info.h" @ONLY
)

# Engine sources
set(ENGINE_SOURCES
    "src/engine/shader_loader.cpp"
    "src/engine/time.cpp"
    "src/engine/scene.cpp"
    "src/engine/renderer.cpp"
    "src/engine/gui/debug_gui.cpp"
    "src/engine/window_utils.cpp"
    "src/engine/vertex.cpp"
    "src/engine/mesh.cpp"
)

# Shader sources; we need to track their changes
set(SHADER_SOURCES
    "src/engine/shader/basic.vert.glsl"
    "src/engine/shader/basic.frag.glsl"
)
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SHADER_SOURCES}
)
# need to duplicate the source paths, no clean way to re-use afaik
file(READ "src/engine/shader/basic.vert.glsl" SHADER_SRC_VERT_BASIC)
file(READ "src/engine/shader/basic.frag.glsl" SHADER_SRC_FRAG_BASIC)
configure_file(
    "src/engine/shader_sources.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shader_sources.h"
    @ONLY
)

# Scene sources. this is a WIP
set(SCENE_SOURCES "src/scenes/triangle_scene.cpp")


##########################################################
#                   EXECUTABLE TARGET                    #
##########################################################

# Create the executable target.
# Since Dear ImGUI is not being built as a separate static library,
# we'll need to include the sources here.

if(ANDROID)
    # The SDL Java code is hardcoded to load libmain.so on
    # Android, so we need to change EXECUTABLE_NAME
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

# Web targets need CMake to generate a HTML webpage
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# Add sources to the target
target_sources(${EXECUTABLE_NAME} PRIVATE
    "src/main.cpp"
    "glad/src/glad.c"
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${ENGINE_SOURCES}
    ${SCENE_SOURCES}
    # TODO: Apple iOS support via src/iosLaunchScreen.storyboard
)

# For debugging
target_sources(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:${IMGUI_DEBUG}>)
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    target_link_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
    # elseif (CMAKE_BUILD_TYPE STREQUAL "Debug"
    #         OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    #     message("Charcoal: Building with build-type ${CMAKE_BUILD_TYPE}...")
    #     if (MINGW)
    #         message("Charcoal: **MINGW toolchain detected**")
    #         # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -gdwarf-4")
    #         # target_compile_options(Charcoal PRIVATE $<$<CONFIG:Debug>:"-g -ggdb -gdwarf-4">)
    #         target_compile_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:-ggdb -gdwarf-4>)
    #         get_target_property(flags ${EXECUTABLE_NAME} COMPILE_OPTIONS)
    #         message(STATUS "Charcoal compile options: ${flags}")
    #     endif()
endif()

# Link to the actual SDL3 library.
# target_link_libraries(Charcoal PUBLIC SDL3_image::SDL3_image)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC
    SDL3_image::SDL3_image
    SDL3::SDL3
)
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_23)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# SDL_image bug: https://github.com/libsdl-org/SDL_image/issues/506
if (APPLE AND NOT BUILD_SHARED_LIBS)
    find_library(IO_LIB ImageIO REQUIRED)
    find_library(CS_LIB CoreServices REQUIRED)
    find_library(CT_LIB CoreText REQUIRED)
    find_library(CG_LIB CoreGraphics REQUIRED)
    find_library(CF_LIB CoreFoundation REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB})
endif()

# Pick up the includes for our dependencies
# target_include_directories(${EXECUTABLE_NAME} PRIVATE "${sdl3_SOURCE_DIR}/include")
# target_include_directories(${EXECUTABLE_NAME} PRIVATE "${sdl3_image_SOURCE_DIR}/include")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}/backends")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}/misc/cpp")
# target_include_directories(${EXECUTABLE_NAME} PRIVATE "glm")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "glad/include")

# Link to the library (GLM becomes a static library in this case)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC glm::glm-header-only)

# Installation rules
include(GNUInstallDirs)
# include(InstallRequiredSystemLibraries)
install(TARGETS
            ${EXECUTABLE_NAME}
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (TARGET SDL3-shared)
    message("Configuring SDL-shared installation")
    install(TARGETS
                SDL3-shared
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if (TARGET SDL3-static)
    message("Configuring SDL-static installation")
    install(TARGETS
                SDL3-static
        RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

# RPATH mangling
# TODO (figure out how we're meant to do installs on Linux)


##########################################################
#                        RESOURCES                       #
##########################################################

# Copy resources
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/resources"
            "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resources"
)




# The CMake docs have some examples:
# https://cmake.org/cmake/help/latest/command/install.html#example-install-targets-with-per-artifact-components
# https://cmake.org/cmake/help/latest/command/install.html#example-install-targets-to-per-config-destinations
# install(TARGETS Charcoal
#         CONFIGURATIONS Release
#         RUNTIME DESTINATION "bin")
