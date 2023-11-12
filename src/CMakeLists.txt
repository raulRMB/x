add_executable(${PROJECT_NAME})

set (SOURCES
        Base/Defines.h

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

        Core/Mesh.h Core/Mesh.cpp
        Core/Game.h Core/Game.cpp
        Core/Scene.h Core/Scene.cpp
        Core/Camera.h Core/Camera.cpp
        Core/MeshModel.cpp Core/MeshModel.h
        Core/SkeletalMesh.cpp Core/SkeletalMesh.h

        Engine/main.cpp
        Engine/Renderer/Base/RendererInstance.h Engine/Renderer/Base/RendererInstance.cpp
        Engine/Renderer/Base/RendererDevice.h Engine/Renderer/Base/RendererDevice.cpp
        Engine/Renderer/Base/SwapChain.h Engine/Renderer/Base/SwapChain.cpp
        Engine/Renderer/Renderer.h Engine/Renderer/Renderer.cpp
        Engine/Window.h Engine/Window.cpp
        Engine/Engine.h Engine/Engine.cpp
        Engine/Renderer/RendererUtil.h

        Enums/SceneId.h

        Navigation/Navigation.h Navigation/Navigation.cpp

        Network/NetMsgType.h
        Network/NetMessage.h
        Network/NetCompId.h
        Network/NetworkDriver.h Network/NetworkDriver.cpp

        Scenes/TestScene.h Scenes/TestScene.cpp
        Scenes/MainScene.h Scenes/MainScene.cpp

        Util/Primitives.h
        Util/Color.h

        Util/Util.h Util/Util.cpp
        Util/File.h Util/File.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)