# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# resource files compiler (unused for now)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

# static C++ and GCC runtimes
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -static-libstdc++ -static-libgcc")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -static-libstdc++ -static-libgcc")

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/bin/x86_64-w64-mingw32-g++ ${CMAKE_FIND_ROOT_PATH})

# adjust the default behavior of the FIND_XXX() commands:
## search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

## search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

## search packages in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
