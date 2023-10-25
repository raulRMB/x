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
#include "util/Util.h"
#include "Navigation/Navigation.h"
#include <glm/gtc/constants.hpp>

f32 RandomFloat(f32 min, f32 max);

std::vector<v2> verts;
void MainScene::Start()
{
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Red);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Green);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Blue);

    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    CTriangleMesh x{x::Renderer::Get().CreateMeshModel("../models/map.obj")};
    AddComponent(e, x);
    Entities.push_back(e);

    FollowEntity = CreateEntity();
    transform.WorldPosition = {251.0f, .0f, -251.0f};
    transform.WorldRotation = {0.f, 0.f, 0.0f};
    transform.WorldScale = v3(2.0f);
    AddComponent(FollowEntity, transform);
    AddComponent(FollowEntity, CTriangleMesh(x::Renderer::Get().CreateMeshModel("../models/ball.fbx")));
    Entities.push_back(FollowEntity);

    TargetEntity = CreateEntity();
    transform.WorldPosition = {251.0f, .0f, -251.0f};
    AddComponent(TargetEntity, transform);
    Entities.push_back(FollowEntity);

    SDL_SetWindowMouseGrab(x::Window::Get().GetWindow(), SDL_TRUE);
}

void MainScene::Update(f32 deltaTime)
{
    Scene::Update(deltaTime);

    i32 x, y;
    SDL_GetMouseState(&x, &y);

    f32 speed = 200.f;

    if(x < 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(-speed, 0.0f, 0.0f) * deltaTime);
    }
    else if(x > WINDOW_WIDTH - 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(speed, 0.0f, 0.0f) * deltaTime);
    }

    if(y < 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, -speed) * deltaTime);
    }
    else if(y > WINDOW_HEIGHT - 25)
    {
        CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, speed) * deltaTime);
    }

    v3& FollowPos = Registry.get<CTransform3d>(FollowEntity).WorldPosition;
    v3& targetPos = Registry.get<CTransform3d>(TargetEntity).WorldPosition;

    if(glm::distance(FollowPos, targetPos) > 0.01f)
    {
        FollowPos += glm::normalize(targetPos - FollowPos) * 50.0f * deltaTime;
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
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        v3 start = CameraSystem::Get().GetMainCameraPosition();
        v3 end = x::RenderUtil::GetMouseWorldPosition();
        v3 point = x::Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
        verts.emplace_back(point.x, point.z);
        GetComponent<CTransform3d>(TargetEntity).WorldPosition = point;
        auto e = CreateEntity();
        CTransform3d transform{};
        transform.WorldPosition = point;
        transform.WorldRotation.x = glm::radians(90.f);
        transform.WorldScale = v3(1.2f);
        AddComponent(e, transform);
        AddComponent(e, CLineMesh(0));
    }
    else if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_g)
        {
            auto view = Registry.view<CLineMesh>();
            for(auto entity : view)
            {
                RemoveEntity(entity);
            }

            x::Scene* s = x::Game::GetInstance().GetScene();
            for(const Triangle2D& triangle : x::Navigation::BowyerWatson(verts))
            {
                auto e = s->CreateEntity();
                s->AddComponent(e, CTransform3d());
                s->AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], x::Color::White)));
                CTransform3d transform{};
            }
        }
    }
}