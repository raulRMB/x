//
// Created by Raul Romero on 2023-09-22.
//

#include "renderer.h"


s32 renderer::Init(GLFWwindow *window)
{
    Window = window;

    try
    {
        CreateInstance();
    } catch (const std::runtime_error& e)
    {
        printf("Error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

renderer::renderer() : Window(nullptr), Instance(VkInstance()) {}

renderer::~renderer()
{
    Window = nullptr;
}

void renderer::CreateInstance()
{

}
