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
        DeltaTime(0.f)
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

            MoveCamera(event);

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

    void Engine::MoveCamera(const SDL_Event& event)
    {
        if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_w)
            {
                mask |= 1u << 0;
            }
            if(event.key.keysym.sym == SDLK_s)
            {
                mask |= 1u << 1;
            }
            if(event.key.keysym.sym == SDLK_a)
            {
                mask |= 1u << 2;
            }
            if(event.key.keysym.sym == SDLK_d)
            {
                mask |= 1u << 3;
            }
        }
        if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == SDLK_w)
            {
                mask &= ~(1u << 0);
            }
            if(event.key.keysym.sym == SDLK_s)
            {
                mask &= ~(1u << 1);
            }
            if(event.key.keysym.sym == SDLK_a)
            {
                mask &= ~(1u << 2);
            }
            if(event.key.keysym.sym == SDLK_d)
            {
                mask &= ~(1u << 3);
            }
        }
        if(event.type == SDL_MOUSEWHEEL)
        {
            if(event.wheel.y > 0)
            {
                Renderer.Camera.Position.z -= 1.f;
            }
            else if(event.wheel.y < 0)
            {
                Renderer.Camera.Position.z += 1.f;
            }
        }

        v3 pos = Renderer.Camera.Position;
        f32 speed = 1.f;

        if(mask & 1u << 0)
        {
            pos.y -= speed * DeltaTime.count();
        }
        if(mask & 1u << 1)
        {
            pos.y += speed * DeltaTime.count();
        }
        if(mask & 1u << 2)
        {
            pos.x -= speed * DeltaTime.count();
        }
        if(mask & 1u << 3)
        {
            pos.x += speed * DeltaTime.count();
        }
        Renderer.UpdateCamera(pos);
    }
}