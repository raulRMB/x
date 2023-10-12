//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"
#include "SDL_events.h"

#include <glm/gtc/matrix_transform.hpp>
#include "core/Camera.h"

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
        Renderer(x::Renderer()),
        Window(x::Window())
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

            Game::GetInstance().HandleInput(event);

            CameraSystem::Get().MoveCamera(event, DeltaTime.count());

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
}