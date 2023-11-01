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
#include <thread>
#include <fstream>
#include <sstream>

f32 RandomFloat(f32 min, f32 max);

void MainScene::Start()
{
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Red);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Green);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Blue);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Yellow);
    x::Renderer::Get().CreateMesh("1x1.png", x::Primitives2D::Shape::Circle, x::Color::Magenta);

    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    CTriangleMesh x{x::Renderer::Get().CreateMeshModel("../models/map.obj")};
    AddComponent(e, x);
    Entities.push_back(e);

    e = CreateEntity();
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    x = {x::Renderer::Get().CreateMeshModel("../models/Stones.fbx")};
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

    Load();
}

i32 indx = 0;
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
    else
    {
        indx++;
        if(StringPath.size() > indx)
            GetRegistry().get<CTransform3d>(TargetEntity).WorldPosition = {StringPath[indx].x, 0.0f, StringPath[indx].y};
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
        GetRegistry().clear<CLineMesh>();

        v3 start = CameraSystem::Get().GetMainCameraPosition();
        v3 end = x::RenderUtil::GetMouseWorldPosition();
        v3 p = x::Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
        EndPoint = {p.x, p.z};

        auto e = CreateEntity();
        CTransform3d transform{};
        transform.WorldPosition = {p.x, 0.0f, p.z};
        transform.WorldRotation.x = glm::radians(90.f);
        transform.WorldScale = v3(1.2f);
        AddComponent(e, transform);
        AddComponent(e, CLineMesh(1));

        std::vector<TriangleNode*> path;
        StartPoint = {GetComponent<CTransform3d>(FollowEntity).WorldPosition.x, GetComponent<CTransform3d>(FollowEntity).WorldPosition.z};

        x::Navigation::AStar(StartPoint, EndPoint, path, Portals, Tris);

        StringPath = x::Navigation::StringPull(Portals, StartPoint, EndPoint);

        printf("Portals size: %zu\n", Portals.size());
        printf("String path size: %zu\n", StringPath.size());
        Portals.clear();

        for(TriangleNode* node : path)
        {
            const Triangle2D& triangle = node->GetTriangle();
            e = CreateEntity();
            AddComponent(e, CTransform3d());
            AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], x::Color::Green)));
        }

        for(i32 i = 0; i < StringPath.size() - 1; i++)
        {
            e = CreateEntity();
            v3 p1 = {StringPath[i].x, 0.0f, StringPath[i].y};
            v3 p2 = {StringPath[i + 1].x, 0.0f, StringPath[i + 1].y};
            AddComponent(e, CTransform3d());
            AddComponent(e, CLineMesh(x::Renderer::Get().CreateLine(p1, p2, x::Color::Cyan)));
        }

        indx = 1;
        GetRegistry().get<CTransform3d>(TargetEntity).WorldPosition = {StringPath[indx].x, 0.0f, StringPath[indx].y};
    }
    else if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_g)
        {
//            auto view = Registry.clear<CLineMesh>();

            x::Scene* s = x::Game::GetInstance().GetScene();
            Tris = x::Navigation::BowyerWatson(points);

            for(const TriangleNode& node : Tris)
            {
                auto e = s->CreateEntity();
                s->AddComponent(e, CTransform3d());
                const Triangle2D& triangle = node.GetTriangle();
                s->AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], node.IsBlocked() ? x::Color::Red : x::Color::White)));
                CTransform3d transform{};
            }
        }
        if(event.key.keysym.sym == SDLK_c)
        {
            v3 start = CameraSystem::Get().GetMainCameraPosition();
            v3 end = x::RenderUtil::GetMouseWorldPosition();
            v3 p = x::Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
            for(const TriangleNode& graphTriangle : Tris)
            {
                v2 point = {p.x, p.z};
                const Triangle2D& triangle = graphTriangle.GetTriangle();
                if(x::Navigation::PointInTriangle(point, triangle))
                {
                    const_cast<TriangleNode&>(graphTriangle).SetBlocked(!graphTriangle.IsBlocked());

                    auto e = CreateEntity();
                    CTransform3d transform{};
                    AddComponent(e, transform);
                    AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], graphTriangle.IsBlocked() ? x::Color::Cyan : x::Color::White)));
                    break;
                }
            }
        }
        if(event.key.keysym.sym == SDLK_h)
        {
            v3 start = CameraSystem::Get().GetMainCameraPosition();
            v3 end = x::RenderUtil::GetMouseWorldPosition();
            v3 p = x::Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
            StartPoint = {p.x, p.z};

            auto e = CreateEntity();
            CTransform3d transform{};
            transform.WorldPosition = {p.x, 0.0f, p.z};
            transform.WorldRotation.x = glm::radians(90.f);
            transform.WorldScale = v3(1.2f);
            AddComponent(e, transform);
            AddComponent(e, CLineMesh(0));
        }
    }
}

void MainScene::Save()
{
    std::ofstream file;
    file.open("save.txt");
    for(const v2& point : points)
    {
        file << point.x << " " << point.y << "\n";
    }

    file << "TRIANGLES\n";

    for(const TriangleNode& graphTriangle : Tris)
    {
        file << graphTriangle.GetIndex() << " ";
        file << graphTriangle.IsBlocked() << "\n";
    }

    file.close();
}

void MainScene::Load()
{
    std::ifstream file;
    file.open("save.txt");
    if(file.is_open())
    {
        std::string line;
        while(std::getline(file, line) && line != "TRIANGLES")
        {
            std::stringstream ss(line);
            f32 x, y;
            ss >> x >> y;
            points.emplace_back(x, y);
        }

        Tris = x::Navigation::BowyerWatson(points);

        while(std::getline(file, line))
        {
            std::stringstream ss(line);
            u32 idx;
            bool blocked;
            ss >> idx >> blocked;
            Tris[idx].SetBlocked(blocked);
        }

        file.close();

//        x::Scene* s = x::Game::GetInstance().GetScene();
//        for(const TriangleNode& graphTriangle : Tris)
//        {
//            auto e = s->CreateEntity();
//            s->AddComponent(e, CTransform3d());
//            const Triangle2D& triangle = graphTriangle.GetTriangle();
//            s->AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], graphTriangle.IsBlocked() ? x::Color::Red : x::Color::White)));
//            CTransform3d transform{};
//        }
    }
}
