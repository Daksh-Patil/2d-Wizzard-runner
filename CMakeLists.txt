cmake_minimum_required(VERSION 3.16)
project(FinalGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for VS Code IntelliSense perfectly
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Find SFML ────────────────────────────────────────────────────────────────
# Supports MSYS2 UCRT64 (default) and system installs
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# ── Collect sources ──────────────────────────────────────────────────────────
file(GLOB SOURCES "src/*.cpp")
add_executable(FinalGame ${SOURCES})

target_include_directories(FinalGame PRIVATE src)

target_link_libraries(FinalGame
    sfml-graphics
    sfml-window
    sfml-system
)

# ── Output the exe into the project root so relative asset paths work ────────
set_target_properties(FinalGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Multi-config generators (VS, Xcode) need this too
foreach(CFG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(FinalGame PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CFG} "${CMAKE_SOURCE_DIR}"
    )
endforeach()

message(STATUS "Sources found: ${SOURCES}")
message(STATUS "Executable will be placed in: ${CMAKE_SOURCE_DIR}")
