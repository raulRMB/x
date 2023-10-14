//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_WINDOW_H
#define R_WINDOW_H

#include <stdexcept>
#include <vector>
#include <iostream>

#include "base/defines.h"
#include "SDL_events.h"

namespace x
{
    class Window
    {
    public:
        static Window &Get();
    private:
        friend class Engine;

#ifdef X_WINDOWING_API_GLFW
        class GLFWwindow* GLFWWindow;
#endif

#ifdef X_WINDOWING_API_SDL
        class SDL_Window *SDLWindow;
        class SDL_Renderer *SDLRenderer;
#endif

    public:
#ifdef X_WINDOWING_API_GLFW
        [[nodiscard]] GLFWwindow* GetWindow() const { return GLFWWindow; }
#endif

#ifdef X_WINDOWING_API_SDL

        [[nodiscard]] SDL_Window *GetWindow() const
        { return SDLWindow; }

#endif

    private:
        Window();

        ~Window();

        i32 Init();

        bool bRunning(SDL_Event &event);

        void Clean();

        i32 InitSDL();

        bool bSDLRunning(SDL_Event &event);

        void CleanSDL();

        i32 InitGLWF();

        bool bGLFWRunning();

        void CleanGLFW();

        void SetFPS(int i);
    };
}

#endif //R_WINDOW_H
