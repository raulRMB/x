//
// Created by Raul Romero on 2023-10-11.
//

#include "MainScene.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/TargetComponent.h"
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
    physics.Velocity = {0.0f, 0.0f};
    AddComponent(e, physics);
    x::Engine::GetInstance().CreateMesh("1x1.png", X::Primitives2D::Shape::Circle, X::Color::Green);
    Entities.push_back(e);
}

void MainScene::HandleInput(const SDL_Event &event)
{
    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            const auto& view = Registry.view<CTarget>();
            f32 x = (f32)event.button.x;
            f32 y = (f32)event.button.y;

            double x_ndc = (2.0f * x / (f32)WINDOW_WIDTH) - 1.f;
            double y_ndc = (2.0f * y / (f32)WINDOW_HEIGHT) - 1.f;
            m4 viewProjectionInverse = x::Engine::GetRenderer().GetViewProjectionInverse();
            v4 worldSpacePosition(x_ndc, y_ndc, 0.0f, 1.0f);
            auto world = viewProjectionInverse * worldSpacePosition;

            x = world.x * abs(Game::GetCamera.Position.z);
            y = world.y * abs(Game::Camera.Position.z);

            for(auto entity : view)
            {
                Registry.get<trgt>(entity).XY = glm::vec2(x, y);
            }
            auto e = registry.create();
            Registry.emplace<pos>(e, glm::vec2(x, y));
            Registry.emplace<modelId>(e, ECSManager::GetInstance().models);
            Registry.emplace<trgt>(e, glm::vec2(x, y));
            Registry.emplace<vel>(e, glm::vec2(0.f, 0.f));
            ECSManager::GetInstance().models++;
            Renderer.CreateMesh();
        }
    }
}

void MainScene::Update(f32 deltaTime)
{
    auto view = Registry.view<CTransform, CPhysics>();
    for (auto entity : view)
    {
        auto& transform = view.get<CTransform>(entity);
        auto& physics = view.get<CPhysics>(entity);

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
