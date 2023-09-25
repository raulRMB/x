//
// Created by Raul Romero on 2023-09-23.
//

#ifndef R_WINDOW_H
#define R_WINDOW_H

#include <stdexcept>
#include <vector>
#include <iostream>

#include "base/defines.h"




class xWindow
{
private:
    friend class xEngine;

#ifdef X_WINDOWING_API_GLFW
    class GLFWwindow* GLFWWindow;
#endif

#ifdef X_WINDOWING_API_SDL
    class SDL_Window* SDLWindow;
#endif

public:
#ifdef X_WINDOWING_API_GLFW
    [[nodiscard]] GLFWwindow* GetWindow() const { return GLFWWindow; }
#endif

#ifdef X_WINDOWING_API_SDL
    [[nodiscard]] SDL_Window* GetWindow() const { return SDLWindow; }
#endif

private:
    xWindow();
    ~xWindow();

    i32 Init();
    bool bRunning();
    void Clean();

    i32 InitSDL();
    bool bSDLRunning();
    void CleanSDL();

    i32 InitGLWF();
    bool bGLFWRunning();
    void CleanGLFW();
};


#endif //R_WINDOW_H
