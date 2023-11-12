#ifndef R_WINDOW_H
#define R_WINDOW_H

#include <stdexcept>
#include <vector>
#include <iostream>

#include "../Core/defines.h"
#include "SDL_events.h"

    class Window
    {
    public:
        static Window &Get();
    private:
        friend class Engine;

        class SDL_Window *SDLWindow;

    public:
        [[nodiscard]] SDL_Window *GetWindow() const
        { return SDLWindow; }

    private:
        Window();

        ~Window();

        i32 Init();

        bool bRunning(SDL_Event &event);

        void Clean();

        i32 InitSDL();

        bool bSDLRunning(SDL_Event &event);

        void CleanSDL();

        void SetFPS(int i);
    };

#endif //R_WINDOW_H
