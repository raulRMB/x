//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"

xEngine &xEngine::GetInstance()
{
    static xEngine instance;
    return instance;
}

xEngine::xEngine() : Window(xWindow()), Renderer(xRenderer())
{
    Initialized = Window.Init();
    Initialized = Initialized & Renderer.Init(Window.Window);
}

xEngine::~xEngine()
{
    Renderer.Clean();
    Window.Clean();
}

i32 xEngine::Run()
{
    if (Initialized != EXIT_SUCCESS)
        return EXIT_FAILURE;

    Window.Loop();

    return EXIT_SUCCESS;
}
