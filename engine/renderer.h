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
private:
    friend class engine;

    GLFWwindow* Window;

    VkInstance Instance;

private:
    renderer();
    ~renderer();
    s32 Init(GLFWwindow* window);
    void CreateInstance();
};


#endif //R_RENDERER_H
