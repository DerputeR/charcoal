mkdir -Force build &&
cd build &&
cmake -G Ninja .. &&
cmake --build . &&
cd ..


