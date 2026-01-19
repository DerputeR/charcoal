${SCRIPT_DIR}=${PSScriptRoot}

if (!$(Test-Path -Path "${SCRIPT_DIR}/build/default/CMakeCache.txt"))
{
    cmake --preset=default
}

if ($(Test-Path -Path "${SCRIPT_DIR}/build/default/CMakeCache.txt"))
{
    cmake --build --preset=default
}
