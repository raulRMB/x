#include "Engine.h"
#include "SDL_events.h"

#include "Core/Camera.h"
#include <random>
#include "backends/imgui_impl_sdl2.h"
#include "Network/NetworkDriver.h"
#include <thread>

namespace x
{
Engine &Engine::Get()
{
    static Engine instance;
    return instance;
}

Engine::Engine() :
    TotalTime(0.f),
    DeltaTime(0.f)
{}

Engine::~Engine() = default;

i32 Engine::Run(Scene * startingScene)
{
    // NetworkDriver::Get().Init();

    if(x::Window::Get().Init() != EXIT_SUCCESS || x::Renderer::Get().Init() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    Init(startingScene);

    // std::thread networkThread([]()
    // {
    //     while(1)
    //     {
    //         NetworkDriver::Get().Loop();
    //     }
    // });

    LastTime = std::chrono::high_resolution_clock::now();
    SDL_Event event;

    while(Window::Get().bRunning(event))
    {
        Game::GetInstance().HandleInput(event);

        CurrentTime = std::chrono::high_resolution_clock::now();

        DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;
        TotalTime += DeltaTime;

        CalculateFPS();

        CameraSystem::Get().MoveCamera(event, DeltaTime.count());
        CameraSystem::Get().UpdateMainCamera();

        Update(DeltaTime.count());

        ImGui_ImplSDL2_ProcessEvent(&event);

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
    Game::GetInstance().DrawUI();
    ImGui::Render();
    Renderer::Get().DrawFrame();
}

void Engine::Init(Scene* startingScene)
{
    Game::GetInstance().Init(startingScene);
}

void Engine::Clean()
{
    Game::GetInstance().Clean();
    Renderer::Get().Clean();
    Window::Get().Clean();
}

void Engine::CreateMesh(const std::string& path, Shape shape, const v4& color)
{
    Renderer::Get().CreateMesh(path, shape, color);
}

void Engine::Save()
{
    Game::GetInstance().Save();
}

void Engine::Load()
{
    Game::GetInstance().Load();
}

void Engine::CalculateFPS()
{
    FrameCount++;
    if (const long long dt = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - FrameStartTime).count(); dt >= 1)
    {
        FPS = FrameCount / dt;
        FrameCount = 0;
        FrameStartTime = CurrentTime;
    }
}
}