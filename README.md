# Dependencies

## Build system dependencies

- [Git LFS](https://github.com/git-lfs/git-lfs?utm_source=gitlfs_site&utm_medium=installation_link&utm_campaign=gitlfs#installing)
- [CMake 3.31+](https://cmake.org/download/)
- [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started)
- [Ninja](https://ninja-build.org/)
- A C++ compiler supporting C++23. Examples:
    1. g++
    2. clang
    3. MSVC
- Optional (for cross-compiling to Windows): [mingw64](https://www.mingw-w64.org/downloads/)

## Libraries

Unless noted, libraries are handled by vcpkg.

- sdl3 (>= 3.4.0)
- glm (>= 1.0.2)
- imgui (FetchContent; 1.91.9-docking)
- glad (included)

## Graphics API

This project requires OpenGL 3.3 or greater.

# Supported systems

This project has been verified to build and run on:

- Windows 11 (amd64)
- Ubuntu 24.04 LTS (amd64, arm64)

This has not yet been tested on OSX, iOS, Web via Emscripten, or Android.

# Build

1. Setup all [build system dependencies](#build-system-dependencies) first
2. Run one of the build scripts:
    1. `build.ps1` - Native build (preset=default)
    2. `build.sh` - Native build (preset=default)
    3. `build-mingw.sh` - Cross-compile build for Windows (preset=mingw)

## Windows-specific

This project can be built with [Microsoft Visual Studio Community Edition](https://visualstudio.microsoft.com/downloads/), which includes CMake (3.31 in VS2022) and vcpkg.
You will still need Ninja and Git LFS installed.
I still recommend setting up your own installations of CMake and vcpkg in case you want to move away from the IDE.
You can specify in Visual Studio's settings whether you want to use your own installations instead of the pre-packaged ones.

Once installed, simply open the project root directory in Visual Studio.

