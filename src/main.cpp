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
#include "Global.h"
#include "scenes/PlayScene.h"
#include "scenes/MenuScene.h"

int main(int argc, char *argv[]) {

    Application app("Shark Card Game", WINDOW_WIDTH, WINDOW_HEIGHT, false);

    if (!app.init()) {
        return 1;
    }

    app.getSceneManager().addScene("PlayScene", std::make_shared<PlayScene>());
    app.getSceneManager().addScene("MenuScene", std::make_shared<MenuScene>());
    app.getSceneManager().setActiveScene("PlayScene");

    app.run();

    return 0;
}