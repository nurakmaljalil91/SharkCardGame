/**
 * @file    menu_scene.cpp
 * @brief   Source file for the SharkCardGame menu scene.
 * @details This file defines the menu scene used by SharkCardGame after the
 *          migration to the current Cbit2D scene and ECS API.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include "menu_scene.h"

#include <utility>

#include "cbit/core/logger.hpp"
#include "cbit/ecs/components.hpp"

namespace shark_card_game::scenes {

/**
 * @brief Creates the menu scene with a play callback.
 * @param onPlaySelected Callback invoked when the play button is pressed.
 */
MenuScene::MenuScene(std::function<void()> onPlaySelected)
    : _onPlaySelected(std::move(onPlaySelected))
{
}

/**
 * @brief Initializes the menu scene entities.
 */
void MenuScene::initialize()
{
    auto title = world.addGameObject("MenuTitle");
    auto& titleTransform = title.getComponent<cbit::ecs::TransformComponent>();
    titleTransform.position = {640.0F, 180.0F};

    auto& titleText = title.addComponent<cbit::ecs::TextComponent>();
    titleText.content = "Shark Card Game";
    titleText.fontPath = "resources/fonts/Kenney_Future.ttf";
    titleText.fontSize = 64.0F;
    titleText.color = {255, 247, 214, 255};

    auto subtitle = world.addGameObject("MenuSubtitle");
    auto& subtitleTransform = subtitle.getComponent<cbit::ecs::TransformComponent>();
    subtitleTransform.position = {640.0F, 250.0F};

    auto& subtitleText = subtitle.addComponent<cbit::ecs::TextComponent>();
    subtitleText.content = "Drag cards into hand and head slots";
    subtitleText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    subtitleText.fontSize = 24.0F;
    subtitleText.color = {198, 226, 255, 255};

    auto playButton = world.addGameObject("PlayButton");
    auto& playTransform = playButton.getComponent<cbit::ecs::TransformComponent>();
    playTransform.position = {640.0F, 360.0F};

    auto& button = playButton.addComponent<cbit::ecs::ButtonComponent>();
    button.size = {260.0F, 84.0F};
    button.backgroundColor = {17, 43, 78, 255};
    button.hoverColor = {27, 66, 118, 255};
    button.pressedColor = {11, 29, 54, 255};
    button.borderColor = {245, 214, 96, 255};
    button.onClick = _onPlaySelected;

    auto& buttonText = playButton.addComponent<cbit::ecs::TextComponent>();
    buttonText.content = "Play";
    buttonText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    buttonText.fontSize = 32.0F;
    buttonText.color = {255, 255, 255, 255};

    cbit2d::core::Logger::info("SharkCardGame menu scene initialized");
}

/**
 * @brief Updates the menu scene.
 * @param deltaTimeSeconds Elapsed time since the previous frame.
 */
void MenuScene::update(float deltaTimeSeconds)
{
    static_cast<void>(deltaTimeSeconds);
    world.update();
}

} // namespace shark_card_game::scenes
