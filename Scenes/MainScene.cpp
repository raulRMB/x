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

void MainScene::Start()
{
//    entt::entity e = CreateEntity();
//    CTransform3d transform{};
//    transform.Position = {0.0f, 0.0f, 0.0f};
//    transform.Rotation = {0.0f, 0.0f, 0.0f};
//    transform.Scale = {.1f, .1f, .1f};
//    AddComponent(e, transform);
//    AddComponent(e, CMesh{Models++});
//    CPhysics3d physics{};
//    physics.Velocity = {0.0f, 1.0f, 0.0f};
//    AddComponent(e, physics);
//    x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Square, X::Color::Red);
//    Entities.push_back(e);
//
//    transform.Scale = {.03f, .03f, .03f};
//    e = CreateEntity();
//    AddComponent(e, transform);
//    AddComponent(e, CMesh{Models++});
//    physics.Velocity = {0.0f, 0.0f, 0.0f};
//    AddComponent(e, physics);
//    x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Green);
//    Entities.push_back(e);
}

void MainScene::HandleInput(const SDL_Event &event)
{
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            v3 worldPos = xRUtil::GetMouseWorldPosition();
            auto e = Registry.create();
            Registry.emplace<CTransform3d>(e, worldPos, glm::vec3(.01f, .01f, .01f), CameraSystem::Get().GetMainCameraRotation() + v3(0.f, glm::radians(90.f), 0.f));
            Registry.emplace<CMesh>(e, x::Renderer::Get().CreateMeshModel("../models/uh60.obj"));
            Registry.emplace<CPhysics3d>(e, CameraSystem::Get().GetMainCameraForward() * .2f);
//            x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Blue);
            Entities.push_back(e);
        }
    }
}

void MainScene::Update(f32 deltaTime)
{
    auto view = Registry.view<CTransform3d, CPhysics3d>();
    for (auto entity : view)
    {
        auto& transform = view.get<CTransform3d>(entity);
        auto& physics = view.get<CPhysics3d>(entity);

        transform.Position += physics.Velocity * deltaTime;
    }
}

void MainScene::Clean()
{
    for(entt::entity entity : Entities)
    {
        RemoveEntity(entity);
    }
}
