mkdir -p build/mingw &&
    cd build/mingw &&
    cmake -DCMAKE_TOOLCHAIN_FILE=./TC-mingw.cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja ../.. &&
    cmake --build . &&
    cp compile_commands.json ../compile_commands.json
