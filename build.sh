mkdir -p build/linux &&
    cd build/linux &&
    cmake -G Ninja ../.. &&
    cmake --build . &&
    cp compile_commands.json ../../compile_commands.json
