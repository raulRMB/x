//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

#ifdef X_WINDOWING_API_GLFW
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
#endif

#ifdef X_WINDOWING_API_SDL
    #include <SDL2/SDL.h>
#endif

xWindow::xWindow() :
#ifdef X_WINDOWING_API_GLFW
    GLFWWindow(nullptr)
#endif
#ifdef X_WINDOWING_API_SDL
    SDLWindow(nullptr)
#endif
    {}

xWindow::~xWindow() = default;

i32 xWindow::InitGLWF()
{
#ifdef X_WINDOWING_API_GLFW
    if(glfwInit() != GLFW_TRUE)
    {
        std::cout << "GLFW failed to initialize" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if(GLFWWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr); GLFWWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;
}

bool xWindow::bGLFWRunning()
{
    bool bIsRunning = false;
#ifdef X_WINDOWING_API_GLFW
    bIsRunning = !glfwWindowShouldClose(GLFWWindow);

    if(bIsRunning)
    {
        glfwPollEvents();
    }
#endif
    return bIsRunning;
}

void xWindow::CleanGLFW()
{
#ifdef X_WINDOWING_API_GLFW
    glfwDestroyWindow(GLFWWindow);
    glfwTerminate();
#endif
}

i32 xWindow::Init()
{
#ifdef X_WINDOWING_API_GLFW
    return InitGLWF();
#endif

#ifdef X_WINDOWING_API_SDL
    return InitSDL();
#endif
    return EXIT_FAILURE;
}

bool xWindow::bRunning()
{
#ifdef X_WINDOWING_API_GLFW
    return bGLFWRunning();
#endif

#ifdef X_WINDOWING_API_SDL
    return bSDLRunning();
#endif
}

void xWindow::Clean()
{
#ifdef X_WINDOWING_API_GLFW
    CleanGLFW();
#endif

#ifdef X_WINDOWING_API_SDL
    CleanSDL();
#endif
}


i32 xWindow::InitSDL()
{
#ifdef X_WINDOWING_API_SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL failed to initialize" << std::endl;
        return EXIT_FAILURE;
    }

    SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
#endif
    return EXIT_SUCCESS;
}

bool xWindow::bSDLRunning()
{
#ifdef X_WINDOWING_API_SDL
    SDL_Event event;
    if(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            return false;
        }
    }
#endif
    return true;
}

void xWindow::CleanSDL()
{
#ifdef X_WINDOWING_API_SDL
    SDL_DestroyWindow(SDLWindow);
    SDL_Quit();
#endif
}