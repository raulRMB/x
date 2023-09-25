//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"

xEngine &xEngine::GetInstance()
{
    static xEngine instance;
    return instance;
}

xEngine::xEngine() :
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

    while(Window.bRunning())
    {
        Renderer.DrawFrame();
    }

    Renderer.Clean();
    Window.Clean();

    return EXIT_SUCCESS;
}
