cmake_minimum_required(VERSION 3.25)
project(katam-minimap VERSION 0.0.1 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(FetchContent)

# === Raylib ===
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        9b6c09c32f7283e849918aef220ec4fa629af8d2
)

option(BUILD_SHARED_LIBS "Build raylib as a shared library" OFF)
option(GLFW_BUILD_WAYLAND "Build the bundled GLFW with Wayland support" ON)
option(GLFW_BUILD_X11 "Build the bundled GLFW with X11 support" ON)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    option(CUSTOMIZE_BUILD "Show options for customizing your Raylib library build." ON)
else()
    option(CUSTOMIZE_BUILD "Show options for customizing your Raylib library build." OFF)
endif()

FetchContent_MakeAvailable(raylib)

add_executable(katam-minimap
    src/main.c
)

target_include_directories(katam-minimap PUBLIC include)
target_link_libraries(katam-minimap PRIVATE raylib)
