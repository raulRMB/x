//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <base/defines.h>
#include "window.h"
#include "renderer.h"

class engine
{
public:
    static engine& GetInstance();
    s32 Run();
private:
    engine();
    ~engine();

private:
    class window Window;
    class renderer Renderer;

    u8 Initialized : 1;
};


#endif //R_ENGINE_H
