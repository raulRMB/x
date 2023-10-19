//
// Created by Raul Romero on 2023-10-18.
//

#ifndef X_SENGINE_H
#define X_SENGINE_H

#include <base/defines.h>
#include <chrono>
#include <vendor/entt.hpp>

class SEngine
{
private:
    bool bRunning;

    std::chrono::high_resolution_clock::time_point LastTime;
    std::chrono::high_resolution_clock::time_point CurrentTime;
    std::chrono::duration<f32> TotalTime;
    std::chrono::duration<f32> DeltaTime;

public:
    SEngine();
    ~SEngine() = default;

    static SEngine &Get()
    {
        static SEngine instance;
        return instance;
    }

    i32 Run();

private:
    void Start();

    i32 Init();

    void Update(f32 deltaTime);

    void Clean();
};

#endif //X_SENGINE_H
