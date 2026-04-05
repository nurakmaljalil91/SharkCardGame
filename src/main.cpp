/**
 * @file    main.cpp
 * @brief   Entry point for SharkCardGame using the current Cbit2D API.
 * @details This file wires the game menu and play scenes into the engine
 *          application bootstrap.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include <memory>

#include "cbit/core/application.hpp"
#include "scenes/menu_scene.h"
#include "scenes/play_scene.h"

/**
 * @brief Starts SharkCardGame using the current Cbit2D application bootstrap.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument array.
 * @return Process exit code.
 */
int main(int argc, char* argv[])
{
    static_cast<void>(argc);
    static_cast<void>(argv);

    cbit2d::core::Application application;
    const auto createPlayScene = [&application]() {
        return std::make_shared<shark_card_game::scenes::PlayScene>([&application]() {
            application.getSceneManager().setActiveScene("MenuScene");
        });
    };

    application.getSceneManager().addScene(
        "MenuScene",
        std::make_shared<shark_card_game::scenes::MenuScene>([&application, &createPlayScene]() {
            application.getSceneManager().addScene(
                "PlayScene",
                createPlayScene()
            );
            application.getSceneManager().setActiveScene("PlayScene");
        })
    );
    application.getSceneManager().setActiveScene("MenuScene");
    return application.run("Shark Card Game", 1280, 720);
}
