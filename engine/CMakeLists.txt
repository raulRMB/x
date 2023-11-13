project(engine)

set (SOURCES
        Components/PhysicsComponent.h
        Components/TransformComponent.h
        Components/TargetComponent.h
        Components/AxesComponent.h
        Components/ParentComponent.h
        Components/ChildComponent.h
        Components/NetworkComponent.h
        Components/QueuedComponent.h
        Components/FollowComponent.h
        Components/SkeletalMeshComponent.h

        Core/Defines.h
        Core/Mesh.h Core/Mesh.cpp
        Core/Game.h Core/Game.cpp
        Core/Scene.h Core/Scene.cpp
        Core/Camera.h Core/Camera.cpp
        Core/MeshModel.cpp Core/MeshModel.h
        Core/SkeletalMesh.cpp Core/SkeletalMesh.h
        Core/Window.h Core/Window.cpp

        Renderer/Base/RendererInstance.h Renderer/Base/RendererInstance.cpp
        Renderer/Base/RendererDevice.h Renderer/Base/RendererDevice.cpp
        Renderer/Base/SwapChain.h Renderer/Base/SwapChain.cpp
        Renderer/Renderer.h Renderer/Renderer.cpp
        Renderer/RendererUtil.h Renderer/RendererUtil.cpp

        Util/Primitives.h
        Util/Color.h
        Util/Util.h Util/Util.cpp
        Util/File.h Util/File.cpp

        Navigation/Navigation.h Navigation/Navigation.cpp

        Network/NetMsgType.h
        Network/NetMessage.h
        Network/NetCompId.h
        Network/NetworkDriver.h Network/NetworkDriver.cpp

        engine.h engine.cpp
)

add_library(engine ${SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC vendor)