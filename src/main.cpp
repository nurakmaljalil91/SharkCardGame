/**
 * @file main.cpp
 * @brief Main file for the SDL2 application with logging using spdlog.
 *
 * This file contains the main function for an SDL2 application that sets up
 * a window and logs events using the Logger class defined in Logger.h.
 *
 * The application initializes SDL2, creates a window, and enters a main loop
 * where it handles events until the user requests to quit.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-20
 */

#include <cbit2d/core/Application.h>
#include <cbit2d/core/SceneManager.h>
#include "scenes/PlayScene.h"

int main(int argc, char* argv[]) {
    Application app("Shark Card Game", 800, 600);
    if (!app.init()) {
        return 1;
    }


    SceneManager::getInstance().addScene("CustomScene",std::make_shared<PlayScene>());
    SceneManager::getInstance().setActiveScene("CustomScene");

    app.run();

    return 0;
}