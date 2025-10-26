mkdir -Force 'build/win' &&
cd 'build/win' &&
cmake -G Ninja ../.. &&
cmake --build . &&
cp 'compile_commands.json' '../../compile_commands.json' &&
cd ..


