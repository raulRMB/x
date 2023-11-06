//
// Created by Raul Romero on 2023-09-23.
//

#include "window.h"

namespace x
{
#ifdef X_WINDOWING_API_SDL

#include <SDL2/SDL.h>

#endif


Window::Window() :
    SDLWindow(nullptr)
    {}

    Window::~Window() = default;

    i32 Window::Init()
    {
        return InitSDL();
    }

    bool Window::bRunning(SDL_Event &event)
    {
        return bSDLRunning(event);
    }

    void Window::Clean()
    {
        CleanSDL();
    }


    i32 Window::InitSDL()
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cout << "SDL failed to initialize " << std::endl;
            return EXIT_FAILURE;
        }

        SDLWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                     WINDOW_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED);

        return EXIT_SUCCESS;
    }

    bool Window::bSDLRunning(SDL_Event &event)
    {
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
        return true;
    }

    void Window::CleanSDL()
    {
        SDL_DestroyWindow(SDLWindow);
        SDL_DestroyRenderer(SDLRenderer);
        SDL_Quit();
    }

    void Window::SetFPS(int i)
    {
        SDL_SetWindowTitle(SDLWindow, (WINDOW_TITLE + std::string(" | FPS: ") + std::to_string(i)).c_str());
    }

    Window &Window::Get()
    {
        static Window instance;
        return instance;
    }
}
