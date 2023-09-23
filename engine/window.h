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

class window
{
private:
    friend class engine;

    GLFWwindow* Window;

private:
    window();
    ~window();

    void Init();
    void Loop();
    void Clean();
};


#endif //R_WINDOW_H
