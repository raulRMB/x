//
// Created by Raul Romero on 2023-10-11.
//

#include "MainScene.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/TargetComponent.h"
#include "engine/engine.h"
#include "core/Camera.h"
#include "Components/ParentComponent.h"
#include "engine/NetworkDriver.h"


void MainScene::Start()
{
    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    CTriangleMesh x{x::Renderer::Get().CreateMeshModel("../models/map.obj")};
    AddComponent(e, x);
    Entities.push_back(e);

    SDL_SetWindowMouseGrab(x::Window::Get().GetWindow(), SDL_TRUE);

    x::Renderer::Get().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Red);
}

void MainScene::Update(f32 deltaTime)
{
    Scene::Update(deltaTime);

    i32 x, y;
    SDL_GetMouseState(&x, &y);

    if(x < 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(-100.0f, 0.0f, 0.0f) * deltaTime);
    }
    else if(x > WINDOW_WIDTH - 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(100.0f, 0.0f, 0.0f) * deltaTime);
    }

    if(y < 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, -100.0f) * deltaTime);
    }
    else if(y > WINDOW_HEIGHT - 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, 100.0f) * deltaTime);
    }
}

void MainScene::Clean()
{
    for(entt::entity entity : Entities)
    {
        RemoveEntity(entity);
    }
}

void MainScene::HandleInput(const SDL_Event &event)
{
}
