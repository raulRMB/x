//
// Created by Raul Romero on 2023-09-23.
//

#include "engine.h"

engine &engine::GetInstance()
{
    static engine instance;
    return instance;
}

engine::engine() : Window(window()), Renderer(renderer())
{
    Initialized = Window.Init();
    Initialized = Initialized & Renderer.Init(Window.Window);
}

engine::~engine()
{
//    Renderer.Clean();
    Window.Clean();
}

s32 engine::Run()
{
    if (Initialized != EXIT_SUCCESS)
        return EXIT_FAILURE;

    Window.Loop();

    return EXIT_SUCCESS;
}
