add_executable(${PROJECT_NAME})

set (SOURCES 
    base/defines.h base/base.c
    engine/main.cpp
    engine/renderer.h Engine/renderer.cpp
    engine/window.h Engine/window.cpp
    engine/engine.h Engine/engine.cpp
    engine/rutil.h
    Enums/SceneId.h
    Scenes/TestScene.h
    Scenes/TestScene.cpp
    Components/TransformComponent.h
    Components/PhysicsComponent.h
    Scenes/MainScene.cpp
    Scenes/MainScene.h
    Components/TargetComponent.h
    Components/AxesComponent.h
    Components/ParentComponent.h
    Components/ChildComponent.h
    engine/NetworkDriver.cpp
    engine/NetworkDriver.h
    Components/NetworkComponent.h
    Net/NetMsgType.h
    Net/NetMessage.h
    Net/NetCompId.h
    Components/QueuedComponent.h
    Navigation/Navigation.h
    Navigation/Navigation.cpp
    Components/FollowComponent.h
    Components/SkeletalMeshComponent.h
    util/file.cpp util/file.h
    util/primitives.h
    util/color.h
    util/Util.cpp
    util/Util.h
    core/mesh.h core/mesh.cpp
    core/Game.h core/Game.cpp
    core/Scene.h core/Scene.cpp
    core/Camera.h core/Camera.cpp
    core/MeshModel.cpp core/MeshModel.h
    core/SkeletalMesh.cpp core/SkeletalMesh.h
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)