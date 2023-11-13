#include "MainScene.h"
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>
#include <engine.h>
#include <Core/Camera.h>
#include <Util/Util.h>
#include <Navigation/Navigation.h>
#include <Components/FollowComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Renderer/Renderer.h>
#include <Core/Window.h>
#include <thread>
#include <fstream>
#include <sstream>

f32 RandomFloat(f32 min, f32 max);

void MainScene::Start()
{
    x::Renderer::Get().CreateMesh("1x1.png", Shape::Circle, x::Color::Red);
    x::Renderer::Get().CreateMesh("1x1.png", Shape::Circle, x::Color::Green);
    x::Renderer::Get().CreateMesh("1x1.png", Shape::Circle, x::Color::Blue);
    x::Renderer::Get().CreateMesh("1x1.png", Shape::Circle, x::Color::Yellow);
    x::Renderer::Get().CreateMesh("1x1.png", Shape::Circle, x::Color::Magenta);

    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {250.0f, 0.0f, -250.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.5f);
    AddComponent(e, transform);
    x::Renderer::Get().CreateSkeletalMesh("../assets/models/ninja.fbx");
    AddComponent(e, CSkeletalMesh(0));
    Entities.push_back(e);

    e = CreateEntity();
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    AddComponent(e, CTriangleMesh(x::Renderer::Get().CreateMeshModel("../assets/models/map.obj")));
    Entities.push_back(e);

    SDL_SetWindowMouseGrab(x::Window::Get().GetWindow(), SDL_TRUE);

    Load();
}

f32 lifeTime = 0.f;
void MainScene::Update(f32 deltaTime)
{
    Scene::Update(deltaTime);

    if(!CameraSystem::DebugCameraActive())
    {
        i32 x, y;
        SDL_GetMouseState(&x, &y);

        f32 speed = 200.f;
        if (x < 25)
        {
            CameraSystem::Get().AddMainCameraPosition(v3(-speed, 0.0f, 0.0f) * deltaTime);
        } else if (x > WINDOW_WIDTH - 25)
        {
            CameraSystem::Get().AddMainCameraPosition(v3(speed, 0.0f, 0.0f) * deltaTime);
        }

        if (y < 25)
        {
            CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, -speed) * deltaTime);
        } else if (y > WINDOW_HEIGHT - 25)
        {
            CameraSystem::Get().AddMainCameraPosition(v3(0.0f, 0.0f, speed) * deltaTime);
        }
    }

    for(entt::entity entity : FollowEntities)
    {
        v2& targetPos = Registry.get<CFollow>(entity).TargetPos;
        v3 TargetPos3d = {targetPos.x, 0.f, targetPos.y};

        v3& FollowPos = Registry.get<CTransform3d>(entity).WorldPosition;
        CFollow& follow = GetComponent<CFollow>(entity);

        std::vector<v2>& StringPath = follow.StringPath;
        if(!follow.bFollow)
        {
            continue;
        }

        if(glm::distance(FollowPos, TargetPos3d) > 0.01f)
        {
            FollowPos += glm::normalize(TargetPos3d - FollowPos) * 50.0f * deltaTime;
        }
        else
        {
            follow.index++;
            if(StringPath.size() > follow.index)
            {
                targetPos = StringPath[follow.index];
            }
        }
    }
    lifeTime += deltaTime;
    auto view = Registry.view<CTransform3d, CSkeletalMesh>();
    for(entt::entity entity : view)
    {
        CSkeletalMesh& skeletalMeshComp = Registry.get<CSkeletalMesh>(entity);
        CTransform3d& transform = Registry.get<CTransform3d>(entity);
        SkeletalMesh& skeletalMesh = x::Renderer::Get().GetSkeletalMesh(skeletalMeshComp.Id);
        m4 m = glm::mat4(1.0f);
        SkeletalMesh::GetPose(skeletalMesh.GetAnimation(), skeletalMesh.GetRootBone(), lifeTime, skeletalMesh.GetCurrentPose(), m, skeletalMesh.GetGlobalInverseTransform());
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
        v3 end = GetMouseWorldPosition();
        v3 p = Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
        EndPoint = {p.x, p.z};

        auto e = CreateEntity();
        CTransform3d transform{};
        transform.WorldPosition = {p.x, 0.0f, p.z};
        transform.WorldRotation.x = glm::radians(90.f);
        transform.WorldScale = v3(1.2f);
        AddComponent(e, transform);
        AddComponent(e, CLineMesh(1));

        for(const entt::entity& ent : FollowEntities)
        {
            std::vector<Navigation::TriangleNode*> path;
            CFollow& follow = GetComponent<CFollow>(ent);
            follow.bFollow = true;
            follow.index = 1;

            StartPoint = {GetComponent<CTransform3d>(ent).WorldPosition.x, GetComponent<CTransform3d>(ent).WorldPosition.z};
            Navigation::AStar(StartPoint, EndPoint, path, Portals, Tris);

            std::vector<v2>& StringPath = follow.StringPath;

            StringPath = Navigation::StringPull(Portals, StartPoint, EndPoint);
            Portals.clear();

            follow.TargetPos = StringPath[follow.index];
        }
    }
    if(event.type == SDL_KEYDOWN)
    {
        if(event.key.keysym.sym == SDLK_s)
        {
            v3 start = CameraSystem::Get().GetMainCameraPosition();
            v3 end = GetMouseWorldPosition();
            v3 p = Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
            auto e = CreateEntity();
            CTransform3d transform{};
            FollowEntities.emplace_back(e);
            transform.WorldPosition = p;
            transform.WorldRotation = {glm::radians(180.f), 0.f, 0.0f};
            transform.WorldScale = v3(0.1f);
            AddComponent(e, transform);
            AddComponent(e, CSkeletalMesh(0));
            AddComponent(e, CFollow());
            Entities.push_back(e);
        }
        if(event.key.keysym.sym == SDLK_c)
        {
            v3 start = CameraSystem::Get().GetMainCameraPosition();
            v3 end = GetMouseWorldPosition();
            v3 p = Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);
            for(const Navigation::TriangleNode& graphTriangle : Tris)
            {
                v2 point = {p.x, p.z};
                const Triangle2D& triangle = graphTriangle.GetTriangle();
                if(Navigation::PointInTriangle(point, triangle))
                {
                    const_cast<Navigation::TriangleNode&>(graphTriangle).SetBlocked(!graphTriangle.IsBlocked());

                    auto e = CreateEntity();
                    CTransform3d transform{};
                    AddComponent(e, transform);
                    AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], graphTriangle.IsBlocked() ? x::Color::Cyan : x::Color::White)));
                    break;
                }
            }
        }
        if(event.key.keysym.sym == SDLK_z)
        {
            v3 start = CameraSystem::Get().GetMainCameraPosition();
            v3 end = GetMouseWorldPosition();
            v3 p = Util::Intersect(v3(0.0f), v3(0.0f, 1.0f, 0.0f), start, end - start);

            auto e = CreateEntity();
            CTransform3d transform{};
            transform.WorldPosition = p;
            transform.WorldRotation = {90.f, 0.f, 0.0f};
            transform.WorldScale = v3(2.0f);
            AddComponent(e, transform);
            AddComponent(e, CLineMesh(1));
            points.emplace_back(p.x, p.z);
            Entities.push_back(e);
        }
        if(event.key.keysym.sym == SDLK_x)
        {
            for(const Navigation::TriangleNode& graphTriangle : Tris)
            {
                auto e = CreateEntity();
                AddComponent(e, CTransform3d());
                const Triangle2D& triangle = graphTriangle.GetTriangle();
                AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], graphTriangle.IsBlocked() ? x::Color::Red : x::Color::White)));
                CTransform3d transform{};
            }
        }
        if(event.key.keysym.sym == SDLK_F1)
        {
            bShowUI = !bShowUI;
        }
    }
}

void MainScene::Save()
{
    std::ofstream file;
    file.open("../assets/save.txt");
    for(const v2& point : points)
    {
        file << point.x << " " << point.y << "\n";
    }

    file << "TRIANGLES\n";

    for(const Navigation::TriangleNode& graphTriangle : Tris)
    {
        file << graphTriangle.GetIndex() << " ";
        file << graphTriangle.IsBlocked() << "\n";
    }

    file.close();
}

void MainScene::Load()
{
    std::ifstream file;
    file.open("../assets/save.txt");
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

        Tris = Navigation::BowyerWatson(points);

        while(std::getline(file, line))
        {
            std::stringstream ss(line);
            u32 idx;
            bool blocked;
            ss >> idx >> blocked;
            Tris[idx].SetBlocked(blocked);
        }

        file.close();

        for(const Navigation::TriangleNode& graphTriangle : Tris)
        {
            auto e = CreateEntity();
            AddComponent(e, CTransform3d());
            const Triangle2D& triangle = graphTriangle.GetTriangle();
            if(graphTriangle.IsBlocked())
            {
                AddComponent(e, CLineMesh(x::Renderer::Get().CreateTriangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], x::Color::Red)));
            }
            CTransform3d transform{};
        }
    }
}

void MainScene::DrawUI()
{
    ImGui::NewFrame();

    if(bShowUI)
    {
        static u32 fps = x::Engine::GetFPS();
        ImGui::Text("FPS: %d", fps);

        if(ImGui::Button("Save"))
        {
            Save();
        }
        if(ImGui::Button("Load"))
        {
            Load();
        }
    }

    ImGui::EndFrame();
}
