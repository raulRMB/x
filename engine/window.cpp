//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

namespace x
{
#ifdef X_WINDOWING_API_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#ifdef X_WINDOWING_API_SDL

#include <SDL2/SDL.h>

#endif


    Window::Window() :
#ifdef X_WINDOWING_API_GLFW
            GLFWWindow(nullptr)
#endif
#ifdef X_WINDOWING_API_SDL
            SDLWindow(nullptr)
#endif
    {}

    Window::~Window() = default;

    i32 Window::InitGLWF()
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

    bool Window::bGLFWRunning()
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

    void Window::CleanGLFW()
    {
#ifdef X_WINDOWING_API_GLFW
        glfwDestroyWindow(GLFWWindow);
        glfwTerminate();
#endif
    }

    i32 Window::Init()
    {
#ifdef X_WINDOWING_API_GLFW
        return InitGLWF();
#endif

#ifdef X_WINDOWING_API_SDL
        return InitSDL();
#endif
        return EXIT_FAILURE;
    }

    bool Window::bRunning(SDL_Event &event)
    {
#ifdef X_WINDOWING_API_GLFW
        return bGLFWRunning();
#endif

#ifdef X_WINDOWING_API_SDL
        return bSDLRunning(event);
#endif
    }

    void Window::Clean()
    {
#ifdef X_WINDOWING_API_GLFW
        CleanGLFW();
#endif

#ifdef X_WINDOWING_API_SDL
        CleanSDL();
#endif
    }


    i32 Window::InitSDL()
    {
#ifdef X_WINDOWING_API_SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cout << "SDL failed to initialize" << std::endl;
            return EXIT_FAILURE;
        }

        SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                     WINDOW_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);
#endif
        return EXIT_SUCCESS;
    }

    bool Window::bSDLRunning(SDL_Event &event)
    {
#ifdef X_WINDOWING_API_SDL
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return false;
                }
            }
        }
#endif
        return true;
    }

    void Window::CleanSDL()
    {
#ifdef X_WINDOWING_API_SDL
        SDL_DestroyWindow(SDLWindow);
        SDL_DestroyRenderer(SDLRenderer);
        SDL_Quit();
#endif
    }

    void Window::SetFPS(int i)
    {
#ifdef X_WINDOWING_API_SDL
        SDL_SetWindowTitle(SDLWindow, (WINDOW_TITLE + std::string(" | FPS: ") + std::to_string(i)).c_str());
#endif
    }
}
