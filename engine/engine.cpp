//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"

#include <glm/gtc/matrix_transform.hpp>

xEngine &xEngine::GetInstance()
{
    static xEngine instance;
    return instance;
}

xEngine::xEngine() :
    TotalTime(0.f),
    DeltaTime(0.f),
    Window(xWindow()),
    Renderer(xRenderer())
    {}

xEngine::~xEngine() = default;

i32 xEngine::Run()
{
    if(Window.Init() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if(Renderer.Init(&Window) != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    LastTime = std::chrono::high_resolution_clock::now();
    while(Window.bRunning())
    {
        CurrentTime = std::chrono::high_resolution_clock::now();
        DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;

        TotalTime += DeltaTime;
        if(TotalTime.count() >= 0.2)
        {
            f64 FPS = 1.0 / static_cast<f64>(DeltaTime.count());
            Window.SetFPS(static_cast<i32>(FPS));
            TotalTime = std::chrono::duration<f32>(0.0);
        }

        Angle += 0.4f * DeltaTime.count();
        Renderer.UpdateModel(0, glm::rotate(glm::mat4(1.f), Angle, glm::vec3(0.f, 0.f, 1.f)));
        Renderer.DrawFrame();
    }

    Renderer.Clean();
    Window.Clean();

    return EXIT_SUCCESS;
}
