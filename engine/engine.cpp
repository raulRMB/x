//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"
#include "SDL_events.h"

#include <glm/gtc/matrix_transform.hpp>
#include "core/Camera.h"
#include "vendor/imgui/backends/imgui_impl_sdl2.h"

namespace x
{
    Engine &Engine::GetInstance()
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine() :
        TotalTime(0.f),
        DeltaTime(0.f)
        {}

    Engine::~Engine() = default;

    i32 Engine::Run()
    {
        if(Window::Get().Init() != EXIT_SUCCESS || x::Renderer::Get().Init() != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }

        Init();

        LastTime = std::chrono::high_resolution_clock::now();
        SDL_Event event;
        while(Window::Get().bRunning(event))
        {

            Game::GetInstance().HandleInput(event);
            CurrentTime = std::chrono::high_resolution_clock::now();
            DeltaTime = CurrentTime - LastTime;
            LastTime = CurrentTime;
            TotalTime += DeltaTime;

            CameraSystem::Get().MoveCamera(event, DeltaTime.count());

            Update(DeltaTime.count());

            ImGui_ImplSDL2_ProcessEvent(&event);
            ImGui_ImplSDL2_NewFrame(Window::Get().GetWindow());
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
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
        ImGui::Render();
        x::Renderer::Get().DrawFrame();
    }

    void Engine::Init()
    {
        Game::GetInstance().Init();
    }

    void Engine::Clean()
    {
        Game::GetInstance().Clean();
        Renderer::Get().Clean();
        Window::Get().Clean();
    }

    void Engine::CreateMesh(const std::string& path, X::Primitives2D::Shape shape, const v4& color)
    {
        x::Renderer::Get().CreateMesh(path, shape, color);
    }
}