//
// Created by Raul Romero on 2023-09-22.
//

#ifndef R_RENDERER_H
#define R_RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "base/defines.h"

#include <stdexcept>
#include <vector>

class renderer {
public:
    renderer() = default;
    ~renderer() = default;

    u16 width = 0;
};


#endif //R_RENDERER_H
