//
// Created by Raul Romero on 2023-10-11.
//

#include "MainScene.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PhysicsComponent.h"
#include "engine/engine.h"

void MainScene::Start()
{
    entt::entity e = CreateEntity();
    CTransform transform{};
    transform.Position = {0.0f, 0.0f};
    transform.Scale = 0.1f;
    transform.Rotation = 0.0f;
    transform.Elevation = 0.0f;
    AddComponent(e, transform);
    AddComponent(e, CMesh{Models++});
    CPhysics physics{};
    physics.Velocity = {0.0f, 0.0f};
    AddComponent(e, physics);
    x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Square, X::Color::Red);
    Entities.push_back(e);
    transform.Position = {0.0f, 0.3f};
    transform.Scale = 0.3f;
    e = CreateEntity();
    AddComponent(e, transform);
    AddComponent(e, CMesh{Models++});
    physics.Velocity = {0.1f, 0.0f};
    AddComponent(e, physics);
    x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Red);
    Entities.push_back(e);
}

f32 t = 0.0f;
void MainScene::Update(f32 deltaTime)
{
    auto view = Registry.view<CTransform, CPhysics>();
    for (auto entity : view)
    {
        auto& transform = view.get<CTransform>(entity);
        auto& physics = view.get<CPhysics>(entity);

        transform.Position += physics.Velocity * deltaTime;
    }

    t += 1.0f * deltaTime;
    pos.x = sin(t) * 2.0f;
    x::Engine::GetInstance().UpdateCamera(pos);
}

void MainScene::Clean()
{
    for(entt::entity entity : Entities)
    {
        RemoveEntity(entity);
    }
}
