##########################################################
#                    PROJECT SETUP                       #
##########################################################

# TODO: reference https://github.com/Ravbug/sdl3-sample/blob/main/CMakeLists.txt
# Update to support Emscripten builds, Apple (need hardware/VM to test)
# Make labeling nicer

cmake_minimum_required(VERSION 3.31...4.0)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")

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

# Windows
if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)
    endif()
endif()


##########################################################
#                      DEPENDENCIES                      #
##########################################################

find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)

# Dear ImGUI doesn't have its own CMakeLists.txt so we'll have to declare
# which sources we're using so we can embed it directly into the executable
# set(IMGUI_SOURCES "${imgui_SOURCE_DIR}/imgui.cpp"
#                   "${imgui_SOURCE_DIR}/imgui_demo.cpp"
#                   "${imgui_SOURCE_DIR}/imgui_draw.cpp"
#                   "${imgui_SOURCE_DIR}/imgui_tables.cpp"
#                   "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
#                   "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
# )
#
# # We will also need to do so for the backend sources
# set(IMGUI_BACKEND_SOURCES "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
#                           "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
# )
#
# # This is recommended for debugging in VS
# set(IMGUI_DEBUG "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natvis"
#                 "${imgui_SOURCE_DIR}/misc/debuggers/imgui.natstepfilter"
# )
#

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
    "src/engine/color.cpp"
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
if(ANDROID)
    # The SDL Java code is hardcoded to load libmain.so on
    # Android, so we need to change EXECUTABLE_NAME
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

# Configure C++ version and extensions
target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_23)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# Web targets need CMake to generate a HTML webpage
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

# Add sources to the target
target_sources(${EXECUTABLE_NAME} PRIVATE
    "src/main.cpp"
    "glad/src/glad.c"
    # ${IMGUI_SOURCES}
    # ${IMGUI_BACKEND_SOURCES}
    ${ENGINE_SOURCES}
    ${SCENE_SOURCES}
    # TODO: Apple iOS support via src/iosLaunchScreen.storyboard
)

# For debugging
target_sources(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:${IMGUI_DEBUG}>)
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
# if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    # target_link_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    # target_compile_options(${EXECUTABLE_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
# endif()

# link libraries
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3_image::SDL3_image)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC imgui::imgui)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC glm::glm-header-only)
target_include_directories(${EXECUTABLE_NAME} PRIVATE "glad/include")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}/backends")
target_include_directories(${EXECUTABLE_NAME} PRIVATE "${imgui_SOURCE_DIR}/misc/cpp")

# Installation rules
# include(GNUInstallDirs)
# install(TARGETS
#             ${EXECUTABLE_NAME}
#         RUNTIME
#             DESTINATION ${CMAKE_INSTALL_BINDIR}
#         LIBRARY
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )
#
# if (TARGET SDL3-shared)
#     message("Configuring SDL-shared installation")
#     install(TARGETS
#                 SDL3-shared
#         RUNTIME
#             DESTINATION ${CMAKE_INSTALL_BINDIR}
#         LIBRARY
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     )
# endif()
#
# if (TARGET SDL3-static)
#     message("Configuring SDL-static installation")
#     install(TARGETS
#                 SDL3-static
#         RUNTIME
#             DESTINATION ${CMAKE_INSTALL_BINDIR}
#         LIBRARY
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE
#             DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     )
# endif()
#
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
    VERBATIM
)

# The CMake docs have some examples:
# https://cmake.org/cmake/help/latest/command/install.html#example-install-targets-with-per-artifact-components
# https://cmake.org/cmake/help/latest/command/install.html#example-install-targets-to-per-config-destinations
# install(TARGETS Charcoal
#         CONFIGURATIONS Release
#         RUNTIME DESTINATION "bin")
