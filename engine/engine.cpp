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
    Window.Init();
//    Renderer.Init(Window);
}

engine::~engine()
{
//    Renderer.Clean();
    Window.Clean();
}

int engine::Run()
{
    Window.Loop();
    return 0;
}
