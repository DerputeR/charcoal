mkdir -Force 'build/win' &&
cd 'build/win' &&
cmake -G Ninja ../.. &&
cmake --build . &&
cd ..


