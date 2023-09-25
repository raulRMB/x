//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

i32 xWindow::Init()
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

xWindow::xWindow() : Window(nullptr) {}

xWindow::~xWindow() = default;

bool xWindow::bIsRunning() const
{
    bool bIsRunning = !glfwWindowShouldClose(Window);

    if(bIsRunning)
    {
        glfwPollEvents();
    }

    return bIsRunning;
}

void xWindow::Clean()
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}
