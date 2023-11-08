//
// Created by Raul Romero on 2023-10-11.
//

#include "Game.h"
#include "Scene.h"
#include "../Scenes/TestScene.h"
#include "../Scenes/MainScene.h"

namespace x
{
    Game &Game::GetInstance()
    {
        static Game instance;
        return instance;
    }

    void Game::Init()
    {
        SetScene(SceneId::MainScene);
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

    void Game::SetScene(SceneId sceneId)
    {
        if (CurrentSceneState != sceneId)
        {
            CurrentSceneState = sceneId;
            switch (sceneId)
            {
                case SceneId::None:
                    CurrentScene = nullptr;
                    break;
                case SceneId::TestScene:
                    CurrentScene = new TestScene();
                    break;
                case SceneId::MainScene:
                    CurrentScene = new MainScene();
                    break;
                default:
                    break;
            }
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
}