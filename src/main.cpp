/**
 * @file    main.cpp
 * @brief   Minimal SharkCardGame bootstrap against the current Cbit2D library.
 * @details This file temporarily replaces the older SharkCardGame bootstrap so
 *          the project can build against the latest Cbit2D engine API first.
 *          The previous implementation is preserved below in comments for
 *          reference while the rest of the game is migrated.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include <memory>

#include <SDL3/SDL.h>

#include "cbit/core/application.hpp"
#include "cbit/core/scene.hpp"

namespace shark_card_game {

/**
 * @brief Minimal placeholder scene used during engine migration.
 */
class BootstrapScene final : public cbit2d::core::Scene {
public:
    /**
     * @brief Initializes the placeholder scene.
     */
    void initialize() override
    {
    }

    /**
     * @brief Updates the placeholder scene.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void update(float deltaTimeSeconds) override
    {
        static_cast<void>(deltaTimeSeconds);
    }

    /**
     * @brief Renders the placeholder scene.
     * @param renderer SDL renderer that owns the current frame.
     */
    void render(SDL_Renderer* renderer) override
    {
        SDL_SetRenderDrawColor(renderer, 8, 30, 52, 255);
        SDL_RenderClear(renderer);
    }
};

} // namespace shark_card_game

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
    application.getSceneManager().addScene("BootstrapScene", std::make_shared<shark_card_game::BootstrapScene>());
    application.getSceneManager().setActiveScene("BootstrapScene");
    return application.run("Shark Card Game", 1280, 720);
}

/*
Previous SharkCardGame bootstrap, preserved during migration to the new Cbit2D API:

#include <cbit2d/Cbit.h>
#include "Global.h"
#include "scenes/PlayScene.h"
#include "scenes/MenuScene.h"

int main(int argc, char *argv[]) {

    Application app("Shark Card Game", GLOBAL_WINDOW_WIDTH, GLOBAL_WINDOW_HEIGHT, false);

//    app.showEditor();
    app.showFps();

    if (!app.init()) {
        return 1;
    }

    app.getSceneManager().addScene("PlayScene", std::make_shared<PlayScene>());
    app.getSceneManager().addScene("MenuScene", std::make_shared<MenuScene>());
    app.getSceneManager().setActiveScene("MenuScene");

    app.run();

    return 0;
}
*/
