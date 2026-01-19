#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

if [[ ! -f "${SCRIPT_DIR}/build/default/CMakeCache.txt" ]]; then
    cmake --preset=default
fi

if [[ -f "${SCRIPT_DIR}/build/default/CMakeCache.txt" ]]; then
    cmake --build --preset=default &&
        cp "${SCRIPT_DIR}/build/mingw/compile_commands.json" "${SCRIPT_DIR}/build/compile_commands.json"
fi
