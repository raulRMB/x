#include "Game.h"

#include <backends/imgui_impl_sdl2.h>

#include "../../src/Scenes/MainScene.h"
#include "../Engine.h"

Game &Game::GetInstance()
{
    static Game instance;
    return instance;
}

void Game::Init(Scene* startingScene)
{
    SetScene(startingScene);
    Start();
}

void Game::Start()
{
    CurrentScene->Start();
}

void Game::Update(f32 deltaTime)
{
    CurrentScene->Update(deltaTime);
}

void Game::HandleInput(const SDL_Event &event)
{
    CurrentScene->HandleInput(event);
}

void Game::Clean()
{
    if(CurrentScene != nullptr)
    {
        CurrentScene->Clean();
    }
}

void Game::SetScene(Scene* scene)
{
    if (CurrentScene != scene)
    {
        CurrentScene = scene;
    }
}

void Game::Save()
{
    CurrentScene->Save();
}

void Game::Load()
{
    CurrentScene->Load();
}

void Game::DrawUI()
{
    ImGui_ImplSDL2_NewFrame(x::Window::Get().GetWindow());
    ImGui::NewFrame();
    CurrentScene->DrawUI();
    ImGui::EndFrame();
    ImGui::Render();
}