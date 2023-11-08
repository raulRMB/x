#ifndef R_ENGINE_H
#define R_ENGINE_H

#include "../base/defines.h"
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

        u8 bShowImGui : 1;
    public:
        Engine(const Engine &) = delete;
        Engine &operator=(const Engine &) = delete;
        static Engine &GetInstance();
        static f32 GetDeltaTime() { return GetInstance().DeltaTime.count(); }

        i32 Run();

    private:
        Engine();

        ~Engine();

        void Init();

        void Update(f32 deltaTime);
        void Clean();
        void Draw();
        void Save();
        void Load();

    public:
        void CreateMesh(const std::string& path, x::Primitives2D::Shape shape, const v4& color);
    };
}


#endif //R_ENGINE_H
