cmake_minimum_required(VERSION 3.26)

project(engine)

set(CMAKE_CXX_STANDARD 17)
option(SDL_TEST OFF)

set (SOURCES
    main.cpp
    renderer.cpp renderer.h
    window.cpp window.h
    engine.cpp engine.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC base #[[SDL3::SDL3]] glfw glm Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PUBLIC base #[[SDL3::SDL3]] glfw glm Vulkan::Vulkan)