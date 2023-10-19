cmake_minimum_required(VERSION 3.26)

project(server)
set(CMAKE_CXX_STANDARD 17)
option(SDL_TEST OFF)

set (SOURCES
        main.cpp
        SEngine.cpp
        SEngine.h
        ../Components/TransformComponent.h
        ../Components/ChildComponent.h
        ../Components/NetworkComponent.h
        ../Components/TargetComponent.h
        ../Components/PhysicsComponent.h
        ../Components/ParentComponent.h
        ../vendor/entt.hpp
        SNetworkDriver.cpp
        SNetworkDriver.h
        SMainScene.cpp
        SMainScene.h
        SNetEntityManager.cpp
        SNetEntityManager.h
        ../Net/NetMsgType.h
        SGame.cpp
        SGame.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE base core ../vendor/enet/include glm SDL2::SDL2)
target_link_directories(${PROJECT_NAME} PRIVATE base enet)
target_link_libraries(${PROJECT_NAME} PRIVATE base core enet glm SDL2::SDL2)
