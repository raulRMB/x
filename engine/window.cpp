//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

s32 window::Init()
{
    if(glfwInit() != GLFW_TRUE)
    {
        std::cout << "GLFW failed to initialize" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if(Window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr); Window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
