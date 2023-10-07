//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <base/defines.h>
#include "window.h"
#include "renderer.h"
#include <chrono>

class xEngine
{
    std::chrono::high_resolution_clock::time_point LastTime;
    std::chrono::high_resolution_clock::time_point CurrentTime;
    std::chrono::duration<f32> TotalTime;
    std::chrono::duration<f32> DeltaTime;

    f32 Angle = 0.f;


public:
    static xEngine& GetInstance();
    i32 Run();
private:
    xEngine();
    ~xEngine();

private:
    class xWindow Window;
    class xRenderer Renderer;
};


#endif //R_ENGINE_H
