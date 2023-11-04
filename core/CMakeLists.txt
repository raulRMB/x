cmake_minimum_required(VERSION 3.26)

project(core)

set(CMAKE_CXX_STANDARD 17)
option(SDL_TEST OFF)

find_package(Vulkan REQUIRED)
add_library(${PROJECT_NAME}
    mesh.h mesh.cpp
    Game.h Game.cpp
    Scene.h Scene.cpp
    Camera.h Camera.cpp
    MeshModel.cpp
    MeshModel.h
        SkeletalMesh.cpp
        SkeletalMesh.h
)

set (ASSIMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/assimp)

target_include_directories(${PROJECT_NAME} PRIVATE base SDL2::SDL2 glfw glm Vulkan::Vulkan ${ASSIMP_DIR}/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE base SDL2::SDL2 glfw glm Vulkan::Vulkan ${ASSIMP_DIR}/lib/libassimp-5.dll)
