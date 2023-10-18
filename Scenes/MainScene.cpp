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

void MainScene::Start()
{
    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(90.f), 0.0f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    CMesh x{x::Renderer::Get().CreateMeshModel("../models/Stronghold.fbx")};
    AddComponent(e, x);
    Entities.push_back(e);

    entt::entity parent = CreateEntity();
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {0.0f, 0.0f, 0.0f};
    transform.WorldScale = v3(1.0f);
    AddComponent(parent, transform);
    x::Renderer::Get().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Red);
    x = {1};
    AddComponent(parent, x);
    CPhysics3d physics{};
    physics.Velocity = {10.0f, 0.0f, 0.0f};
    AddComponent(parent, physics);
    Entities.push_back(parent);

    entt::entity child = CreateEntity();
    transform.LocalPosition = {0.0f, 1.0f, 0.0f};
    AddComponent(child, transform);
    x.Id = 2;
    x::Renderer::Get().CreateMesh("1x1.png", X::Primitives2D::Shape::Square, X::Color::Blue);
    AddComponent(child, x);
    CParent p{};
    p.Children.push_back(child);
    AddComponent(parent, p);
    Entities.push_back(child);
}

void MainScene::HandleInput(const SDL_Event &event)
{
//    if(event.type == SDL_MOUSEMOTION)
//    {
//        if(event.motion.state & SDL_BUTTON_RMASK)
//        {
//            Registry.get<CTransform3d>(Entities[0]).Rotation.y += event.motion.xrel * .01f;
//            Registry.get<CTransform3d>(Entities[0]).Rotation.x += event.motion.yrel * .01f;
//        }
//    }

//    if(event.type == SDL_MOUSEBUTTONDOWN)
//    {
//        if (event.button.button == SDL_BUTTON_LEFT)
//        {
//            v3 worldPos = xRUtil::GetMouseWorldPosition();
//            auto e = Registry.create();
//            Registry.emplace<CTransform3d>(e, worldPos, glm::vec3(.01f, .01f, .01f), CameraSystem::Get().GetMainCameraRotation() + v3(0.f, glm::radians(90.f), 0.f));
//            Registry.emplace<CMesh>(e, model);
//            Registry.emplace<CPhysics3d>(e, CameraSystem::Get().GetMainCameraForward() * .2f);
//            Entities.push_back(e);
//        }
//    }
}

void MainScene::Update(f32 deltaTime)
{
    auto view = Registry.view<CTransform3d, CPhysics3d, CParent>();
    for (auto entity : view)
    {
        auto& transform = view.get<CTransform3d>(entity);
        auto& physics = view.get<CPhysics3d>(entity);
        auto& parent = view.get<CParent>(entity);

        transform.WorldPosition += physics.Velocity * deltaTime;
        for(auto child : parent.Children)
        {
            auto& childTransform = Registry.get<CTransform3d>(child);
            childTransform.WorldPosition = transform.WorldPosition + childTransform.LocalPosition;
        }
    }
}

void MainScene::Clean()
{
    for(entt::entity entity : Entities)
    {
        RemoveEntity(entity);
    }
}
