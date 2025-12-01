cmake_minimum_required(VERSION 4.1.1)

set(PROJECT_NAME voxel_clone)

project(${PROJECT_NAME})

include (FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)

# stb (only header)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

set(SOURCE_FILES 
    src/main.cpp
    src/core/stb_image_impl.cpp
    src/core/game.h
    src/core/game.cpp
    src/core/window.h
    src/core/window.cpp
    src/core/window_events.cpp
    src/core/window_callbacks.cpp
    src/render/shader.h
    src/render/shader.cpp
    src/core/logger.h
    src/core/logger.cpp
    src/render/texture2D.h
    src/render/texture2D.cpp
    src/world/block.h
    src/world/block.cpp
    src/world/chunk.h
    src/world/chunk.cpp
    src/render/texture_atlas.h
    src/render/texture_atlas.cpp
    src/render/camera.hpp
    src/core/input.h
    src/core/input.cpp
    src/render/render.h
    src/render/render.cpp
    src/core/input_handler.h
    src/core/input_handler.cpp
    src/core/resources.h
    src/core/resources.cpp
    src/world/world.h
    src/world/world.cpp
    src/core/loader.h
    src/core/loader.cpp
    src/world/perlin_2d.h
    src/world/perlin_2d.cpp
    src/world/world_generator.h
    src/world/world_generator.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} 
    glfw
    glad
    glm
    spdlog::spdlog
)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})