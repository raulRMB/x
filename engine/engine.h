//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <base/defines.h>
#include "window.h"
#include "renderer.h"
#include <chrono>

namespace x
{
    class Engine
    {
        std::chrono::high_resolution_clock::time_point LastTime;
        std::chrono::high_resolution_clock::time_point CurrentTime;
        std::chrono::duration<f32> TotalTime;
        std::chrono::duration<f32> DeltaTime;
    public:
        Engine(const Engine &) = delete;
        Engine &operator=(const Engine &) = delete;
        static Engine &GetInstance();

        i32 Run();

    private:
        Engine();

        ~Engine();

        void Init();

        void Update(f32 deltaTime);

    private:
        class Window Window;

        class Renderer Renderer;
    };
}


#endif //R_ENGINE_H
