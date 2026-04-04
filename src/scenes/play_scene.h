/**
 * @file    play_scene.h
 * @brief   Header file for the SharkCardGame play scene.
 * @details This file declares the card-table play scene used after migrating
 *          SharkCardGame to the current Cbit2D scene and ECS API.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#ifndef SHARKCARDGAME_PLAY_SCENE_H
#define SHARKCARDGAME_PLAY_SCENE_H

#include <functional>
#include <string_view>

#include <glm/vec2.hpp>

#include "cbit/core/scene.hpp"
#include "../components/components.h"

namespace shark_card_game::scenes {

/**
 * @brief Describes one card face on the tilemap.
 */
struct CardInfo {
    std::string_view name;
    int value = 0;
    glm::vec2 sourcePosition {0.0F, 0.0F};
};

/**
 * @brief Main gameplay scene for SharkCardGame.
 */
class PlayScene final : public cbit2d::core::Scene {
public:
    /**
     * @brief Creates the play scene with a menu callback.
     * @param onReturnToMenu Callback invoked when the menu button is pressed.
     */
    explicit PlayScene(std::function<void()> onReturnToMenu);

    /**
     * @brief Initializes the play scene entities.
     */
    void initialize() override;

    /**
     * @brief Updates the play scene.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void update(float deltaTimeSeconds) override;

private:
    /**
     * @brief Creates all static slot entities for the card table.
     */
    void createBoardSlots();

    /**
     * @brief Creates the deck of draggable cards.
     */
    void createDeck();

    /**
     * @brief Creates one draggable card entity.
     * @param cardInfo Card face information used for the spawned card.
     * @param position Spawn position.
     */
    void createCard(const CardInfo& cardInfo, const glm::vec2& position);

    /**
     * @brief Creates a visual slot on the board.
     * @param tag Readable slot label.
     * @param position Slot center position.
     * @param kind Logical slot role used for snap behavior.
     */
    void createSlot(std::string_view tag, const glm::vec2& position, SlotKind kind);

    /**
     * @brief Returns the front-face tile source for a card.
     * @param card Card component describing the card.
     * @return Source position on the tilemap for the card face.
     */
    static glm::vec2 getCardFrontSourcePosition(const CardComponent& card);

    std::function<void()> _onReturnToMenu;
    float _deltaTimeSeconds = 0.0F;
    glm::vec2 _deckOrigin {0.0F, 0.0F};
};

} // namespace shark_card_game::scenes

#endif //SHARKCARDGAME_PLAY_SCENE_H
