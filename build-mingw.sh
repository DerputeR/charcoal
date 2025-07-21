mkdir -p build &&
    cd build &&
    cmake -DCMAKE_TOOLCHAIN_FILE=./TC-mingw.cmake -G Ninja .. &&
    cmake --build .
