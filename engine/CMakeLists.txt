cmake_minimum_required(VERSION 3.26)

project(engine)

set(CMAKE_CXX_STANDARD 17)
option(SDL_TEST OFF)

set (SOURCES
        main.cpp
        renderer.h renderer.cpp
        window.h window.cpp
        engine.h engine.cpp
        rutil.h
        ../vendor/stb_image.h
        ../Enums/SceneId.h
        ../Scenes/TestScene.h
        ../Scenes/TestScene.cpp
        ../Components/TransformComponent.h
        ../Components/PhysicsComponent.h
        ../Scenes/MainScene.cpp
        ../Scenes/MainScene.h
        ../vendor/entt.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC base util core SDL2::SDL2 glfw glm Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PUBLIC base util core SDL2::SDL2 glfw glm Vulkan::Vulkan)