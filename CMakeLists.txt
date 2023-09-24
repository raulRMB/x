cmake_minimum_required(VERSION 3.26)
project(x)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/Vulkan-Hpp)
add_subdirectory(engine)
add_subdirectory(base)
add_subdirectory(util)