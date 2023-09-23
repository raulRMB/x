//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

void window::Init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    Window = glfwCreateWindow(800, 600, "Radiant", nullptr, nullptr);
}

window::window() : Window(nullptr) {}

window::~window() = default;

void window::Loop()
{
    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
    }
}

void window::Clean()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}
