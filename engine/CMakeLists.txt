cmake_minimum_required(VERSION 3.26)

project(engine)

set(CMAKE_CXX_STANDARD 17)
option(SDL_TEST OFF)

add_executable(${PROJECT_NAME} main.cpp)

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC base SDL3::SDL3 glfw glm Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PUBLIC base SDL3::SDL3 glfw glm Vulkan::Vulkan)
