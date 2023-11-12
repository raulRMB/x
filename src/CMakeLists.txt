add_executable(${PROJECT_NAME})

set (SOURCES

        engine/Components/PhysicsComponent.h
        engine/Components/TransformComponent.h
        engine/Components/TargetComponent.h
        engine/Components/AxesComponent.h
        engine/Components/ParentComponent.h
        engine/Components/ChildComponent.h
        engine/Components/NetworkComponent.h
        engine/Components/QueuedComponent.h
        engine/Components/FollowComponent.h
        engine/Components/SkeletalMeshComponent.h

        engine/Core/Defines.h
        engine/Core/Mesh.h engine/Core/Mesh.cpp
        engine/Core/Game.h engine/Core/Game.cpp
        engine/Core/Scene.h engine/Core/Scene.cpp
        engine/Core/Camera.h engine/Core/Camera.cpp
        engine/Core/MeshModel.cpp engine/Core/MeshModel.h
        engine/Core/SkeletalMesh.cpp engine/Core/SkeletalMesh.h
        engine/Core/Window.h engine/Core/Window.cpp

        engine/Renderer/Base/RendererInstance.h engine/Renderer/Base/RendererInstance.cpp
        engine/Renderer/Base/RendererDevice.h engine/Renderer/Base/RendererDevice.cpp
        engine/Renderer/Base/SwapChain.h engine/Renderer/Base/SwapChain.cpp
        engine/Renderer/Renderer.h engine/Renderer/Renderer.cpp
        engine/Renderer/RendererUtil.h

        engine/Util/Primitives.h
        engine/Util/Color.h

        engine/Util/Util.h engine/Util/Util.cpp
        engine/Util/File.h engine/Util/File.cpp

        engine/Navigation/Navigation.h engine/Navigation/Navigation.cpp

        engine/Network/NetMsgType.h
        engine/Network/NetMessage.h
        engine/Network/NetCompId.h
        engine/Network/NetworkDriver.h engine/Network/NetworkDriver.cpp

        engine/Engine.h engine/Engine.cpp
        
        game/main.cpp
        game/SceneId.h
        game/Scenes/TestScene.h game/Scenes/TestScene.cpp
        game/Scenes/MainScene.h game/Scenes/MainScene.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)