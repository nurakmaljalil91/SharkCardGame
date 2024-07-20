/**
 * @file Application.h
 * @brief Header file for the Application class.
 *
 * This file contains the definition of the Application class which encapsulates
 * the SDL2 application setup, main loop, and cleanup logic.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-20
 */
#ifndef SHARKCARDGAME_APPLICATION_H
#define SHARKCARDGAME_APPLICATION_H


#include <SDL2/SDL.h>

class Application {
public:
    Application();
    ~Application();
    bool init();
    void run();
    void cleanup();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
    SDL_Event e;
};

#endif //SHARKCARDGAME_APPLICATION_H
