cmake_minimum_required(VERSION 3.26)

project(core)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
add_library(${PROJECT_NAME}
        mesh.h mesh.cpp
        Game.h Game.cpp
        Scene.h Scene.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE base glfw glm Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE base glfw glm Vulkan::Vulkan)
