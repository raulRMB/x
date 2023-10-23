cmake_minimum_required(VERSION 3.26)

project(engine)

set(CMAKE_CXX_STANDARD 17)
set (VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../vendor)
option(SDL_TEST OFF)

set (SOURCES
        main.cpp
        renderer.h renderer.cpp
        window.h window.cpp
        engine.h engine.cpp
        rutil.h
        ${VENDOR_DIR}/stb_image.h
        ../Enums/SceneId.h
        ../Scenes/TestScene.h
        ../Scenes/TestScene.cpp
        ../Components/TransformComponent.h
        ../Components/PhysicsComponent.h
        ../Scenes/MainScene.cpp
        ../Scenes/MainScene.h
        ${VENDOR_DIR}/entt.hpp
        ../Components/TargetComponent.h
        ../Components/AxesComponent.h
        ../Components/ParentComponent.h
        ../Components/ChildComponent.h
        NetworkDriver.cpp
        NetworkDriver.h
        ../Components/NetworkComponent.h
        ../Net/NetMsgType.h
        ../Net/NetMessage.h
        ../Net/NetCompId.h
        ../Components/QueuedComponent.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Vulkan REQUIRED)
target_sources(${PROJECT_NAME} PUBLIC
        "${VENDOR_DIR}/imgui/imgui.h"
        "${VENDOR_DIR}/imgui/imgui.cpp"
        "${VENDOR_DIR}/imgui/imgui_demo.cpp"
        "${VENDOR_DIR}/imgui/imgui_draw.cpp"
        "${VENDOR_DIR}/imgui/imgui_widgets.cpp"
        "${VENDOR_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
        "${VENDOR_DIR}/imgui/backends/imgui_impl_sdl2.cpp"
        "${VENDOR_DIR}/imgui/imgui_tables.cpp"
)
target_include_directories(${PROJECT_NAME} PUBLIC
        base util core
        SDL2::SDL2
        glfw glm
        Vulkan::Vulkan
        ${VENDOR_DIR}/assimp/include
        ${VENDOR_DIR}/enet/include
        enet
)

target_link_directories(${PROJECT_NAME} PRIVATE ${VENDOR_DIR}/assimp/lib ${VENDOR_DIR}/enet enet)
target_link_libraries(${PROJECT_NAME} PUBLIC base util core SDL2::SDL2 glfw glm Vulkan::Vulkan ${VENDOR_DIR}/enet/enet64.lib enet)
