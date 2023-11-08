#include "engine.h"
#include "SDL_events.h"

#include <glm/gtc/matrix_transform.hpp>
#include "../core/Camera.h"
#include <random>
#include "backends/imgui_impl_sdl2.h"
#include "NetworkDriver.h"
#include <thread>

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
        NetworkDriver::Get().Init();

        if(Window::Get().Init() != EXIT_SUCCESS || x::Renderer::Get().Init() != EXIT_SUCCESS)
        {
            return EXIT_FAILURE;
        }

        Init();

        std::thread networkThread([]()
        {
            while(1)
            {
                NetworkDriver::Get().Loop();
            }
        });

        LastTime = std::chrono::high_resolution_clock::now();
        SDL_Event event;

        // FPS
        int frameCount = 0;
        int fps = 0;
        auto startTime = std::chrono::high_resolution_clock::now();

        while(Window::Get().bRunning(event))
        {
            Game::GetInstance().HandleInput(event);
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F1)
            {
                bShowImGui = !bShowImGui;
            }

            CurrentTime = std::chrono::high_resolution_clock::now();

            // FPS
            frameCount++;
            auto dt = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - startTime).count();
            if (dt >= 1) {
                fps = frameCount / dt;
                frameCount = 0;
                startTime = CurrentTime;
            }
            
            DeltaTime = CurrentTime - LastTime;
            LastTime = CurrentTime;
            TotalTime += DeltaTime;

            CameraSystem::Get().MoveCamera(event, DeltaTime.count());
            CameraSystem::Get().UpdateMainCamera();

            Update(DeltaTime.count());

            ImGui_ImplSDL2_ProcessEvent(&event);
            ImGui_ImplSDL2_NewFrame(Window::Get().GetWindow());
            ImGui::NewFrame();

            if(bShowImGui)
            {
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

    void Engine::CreateMesh(const std::string& path, x::Primitives2D::Shape shape, const v4& color)
    {
        x::Renderer::Get().CreateMesh(path, shape, color);
    }

    void Engine::Save()
    {
        Game::GetInstance().Save();
    }

    void Engine::Load()
    {
        Game::GetInstance().Load();
    }
}