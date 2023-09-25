//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_WINDOW_H
#define R_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

#include "base/defines.h"

class xWindow
{
private:
    friend class xEngine;

    GLFWwindow* Window;

private:
    xWindow();
    ~xWindow();

    i32 Init();
    bool bIsRunning() const;
    void Clean();
};


#endif //R_WINDOW_H
