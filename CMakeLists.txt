cmake_minimum_required(VERSION 3.25)
project(ReflexMinimap VERSION 0.0.1 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(FetchContent)

# === Raylib ===
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
)

option(BUILD_SHARED_LIBS "Build raylib as a shared library" OFF)
# Unfortunately, it seems Wayland can be broken right now with Raylib v5.5
# Also see https://github.com/raysan5/raylib/issues/4908
option(GLFW_BUILD_WAYLAND "Build the bundled GLFW with Wayland support" OFF)
option(GLFW_BUILD_X11 "Build the bundled GLFW with X11 support" ON)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    option(CUSTOMIZE_BUILD "Show options for customizing your Raylib library build." ON)
else()
    option(CUSTOMIZE_BUILD "Show options for customizing your Raylib library build." OFF)
endif()

FetchContent_MakeAvailable(raylib)

# === ReflexMinimap ===
add_executable(ReflexMinimap
    src/main.c
    src/socket.c
    src/window.c
    src/messageformat.c
    src/pixantiqua.ttf.c
)

target_include_directories(ReflexMinimap PUBLIC include)
target_link_libraries(ReflexMinimap PRIVATE raylib)

target_compile_definitions(ReflexMinimap PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(ReflexMinimap PRIVATE
    $<$<CONFIG:debug>:-O1 -fsanitize=undefined -fno-sanitize-recover=all>
)

target_link_options(ReflexMinimap PRIVATE
    $<$<CONFIG:debug>:-fsanitize=undefined -fno-sanitize-recover=all>
)
