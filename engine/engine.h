//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <base/defines.h>
#include "window.h"
#include "renderer.h"
#include "../core/Game.h"
#include <chrono>

namespace x
{
    class Engine
    {
        std::chrono::high_resolution_clock::time_point LastTime;
        std::chrono::high_resolution_clock::time_point CurrentTime;
        std::chrono::duration<f32> TotalTime;
        std::chrono::duration<f32> DeltaTime;

        u32 mask = 0u;
    public:
        Engine(const Engine &) = delete;
        Engine &operator=(const Engine &) = delete;
        static Engine &GetInstance();

        i32 Run();

        inline static Window& GetWindow() { return Window; }
        inline static Renderer& GetRenderer() { return Renderer; }

    private:
        Engine();

        ~Engine();

        void Init();

        void Update(f32 deltaTime);
        void Draw();
        void Clean();

    private:
        static Window Window;
        static Renderer Renderer;

    public:
        void CreateMesh(const std::string& path, X::Primitives2D::Shape shape, const v4& color);
        void UpdateCamera(const v3& pos);
        void MoveCamera(const SDL_Event& event);
    };
}


#endif //R_ENGINE_H
