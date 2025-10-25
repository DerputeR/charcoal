mkdir -p build/linux &&
    cd build/linux &&
    cmake -G Ninja ../.. &&
    cmake --build .
