//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <base/defines.h>
#include "window.h"
#include "renderer.h"

class xEngine
{
public:
    static xEngine& GetInstance();
    i32 Run();
private:
    xEngine();
    ~xEngine();

private:
    class xWindow Window;
    class xRenderer Renderer;

    u8 Initialized : 1;
};


#endif //R_ENGINE_H
