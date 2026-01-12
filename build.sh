#!/bin/bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

if [[ ! -f "${SCRIPT_DIR}/build/mingw/CMakeCache.txt" ]]; then
    cmake --preset=default
fi

if [[ -f "${SCRIPT_DIR}/build/mingw/CMakeCache.txt" ]]; then
    cmake --build --preset=default
fi
