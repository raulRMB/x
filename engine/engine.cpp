//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"
#include "SDL_events.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

namespace x
{
    Engine &Engine::GetInstance()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() :
        TotalTime(0.f),
        DeltaTime(0.f),
        Window(x::Window()),
        Renderer(x::Renderer())
        {}

    Engine::~Engine() = default;

    i32 Engine::Run()
    {
        if(Window.Init() != EXIT_SUCCESS || Renderer.Init(&Window) != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }

        Init();

        LastTime = std::chrono::high_resolution_clock::now();
        SDL_Event event;
        while(Window.bRunning(event))
        {
            CurrentTime = std::chrono::high_resolution_clock::now();
            DeltaTime = CurrentTime - LastTime;
            LastTime = CurrentTime;
            TotalTime += DeltaTime;

//            if(event.type == SDL_MOUSEBUTTONDOWN)
//            {
//                if (event.button.button == SDL_BUTTON_LEFT)
//                {
//                    auto& registry = ECSManager::GetInstance().GetRegistry();
//                    const auto& view = registry.view<trgt>();
//                    f32 x = (f32)event.button.x;
//                    f32 y = (f32)event.button.y;
//
//                    double x_ndc = (2.0f * x / (f32)WINDOW_WIDTH) - 1.f;
//                    double y_ndc = (2.0f * y / (f32)WINDOW_HEIGHT) - 1.f;
//                    glm::mat4 viewProjectionInverse = glm::inverse(Renderer.GetUBOViewProjection().Projection * Renderer.GetUBOViewProjection().View);
//                    glm::vec4 worldSpacePosition(x_ndc, y_ndc, 0.0f, 1.0f);
//                    auto world = viewProjectionInverse * worldSpacePosition;
//
//                    x = world.x * abs(Renderer.Camera.Position.z);
//                    y = world.y * abs(Renderer.Camera.Position.z);
//
//                    for(auto entity : view)
//                    {
//                        registry.get<trgt>(entity).XY = glm::vec2(x, y);
//                    }
//                    auto e = registry.create();
//                    registry.emplace<pos>(e, glm::vec2(x, y));
//                    registry.emplace<modelId>(e, ECSManager::GetInstance().models);
//                    registry.emplace<trgt>(e, glm::vec2(x, y));
//                    registry.emplace<vel>(e, glm::vec2(0.f, 0.f));
//                    ECSManager::GetInstance().models++;
//                    Renderer.CreateMesh();
//                }
//            }

            Update(DeltaTime.count());
            Draw();
        }

        Clean();

        return EXIT_SUCCESS;
    }

    void Engine::Update(f32 deltaTime)
    {
        Game::GetInstance().Update(deltaTime);
    }

    void Engine::Draw()
    {
        Renderer.DrawFrame();
    }

    void Engine::Init()
    {
        Game::GetInstance().Init();
    }

    void Engine::Clean()
    {
        Game::GetInstance().Clean();
        Renderer.Clean();
        Window.Clean();
    }

    void Engine::CreateMesh(const std::string& path, X::Primitives2D::Shape shape, const v4& color)
    {
        Renderer.CreateMesh(path, shape, color);
    }

    void Engine::UpdateCamera(const v3 &pos)
    {
        Renderer.UpdateCamera(pos);
    }
}