/**
 * @file    menu_scene.h
 * @brief   Header file for the SharkCardGame menu scene.
 * @details This file declares the menu scene used by SharkCardGame after the
 *          migration to the current Cbit2D scene API.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#ifndef SHARKCARDGAME_MENU_SCENE_H
#define SHARKCARDGAME_MENU_SCENE_H

#include <functional>

#include "cbit/core/scene.hpp"

namespace shark_card_game::scenes {

/**
 * @brief Main menu scene for SharkCardGame.
 */
class MenuScene final : public cbit2d::core::Scene {
public:
    /**
     * @brief Creates the menu scene with a play callback.
     * @param onPlaySelected Callback invoked when the play button is pressed.
     */
    explicit MenuScene(std::function<void()> onPlaySelected);

    /**
     * @brief Initializes the menu scene entities.
     */
    void initialize() override;

    /**
     * @brief Updates the menu scene.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void update(float deltaTimeSeconds) override;

private:
    std::function<void()> _onPlaySelected;
};

} // namespace shark_card_game::scenes

#endif //SHARKCARDGAME_MENU_SCENE_H
