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
#include <vector>

#include <SDL3/SDL_audio.h>
#include <glm/vec2.hpp>

#include "cbit/core/scene.hpp"
#include "../gameplay/gameplay_state.h"
#include "../components/components.h"

namespace shark_card_game::scenes {

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
     * @brief Cleans up play-scene owned resources.
     */
    ~PlayScene() override;

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
    struct DealStep {
        cbit::ecs::GameObjectId cardId = 0;
        cbit::ecs::GameObjectId slotId = 0;
    };

    struct ActiveDealAnimation {
        cbit::ecs::GameObjectId cardId = 0;
        cbit::ecs::GameObjectId slotId = 0;
        glm::vec2 startPosition {0.0F, 0.0F};
        glm::vec2 targetPosition {0.0F, 0.0F};
        float elapsedSeconds = 0.0F;
    };

    /**
     * @brief Creates all static slot entities for the card table.
     */
    void createBoardSlots();

    /**
     * @brief Creates the deck of draggable cards.
     */
    void createDeck();

    /**
     * @brief Prepares one hand card and one head card for each player.
     */
    void dealOpeningCards();

    /**
     * @brief Advances the opening deal animation.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void updateDealAnimation(float deltaTimeSeconds);

    /**
     * @brief Starts the next queued card animation if available.
     */
    void beginNextDealStep();

    /**
     * @brief Initializes the lightweight audio stream used for deal ticks.
     */
    void initializeDealAudio();

    /**
     * @brief Releases the lightweight audio stream used for deal ticks.
     */
    void shutdownDealAudio();

    /**
     * @brief Plays one short procedural deal tick.
     */
    void playDealSound();

    /**
     * @brief Places one card entity into a specific slot and applies visibility.
     * @param cardId Card entity to place.
     * @param slotId Target slot entity.
     */
    void placeCardInSlot(cbit::ecs::GameObjectId cardId, cbit::ecs::GameObjectId slotId);

    /**
     * @brief Creates HUD text that reflects the current match state.
     */
    void createMatchHud();

    /**
     * @brief Refreshes the HUD text from the current match state.
     */
    void refreshMatchHud();

    /**
     * @brief Creates one draggable card entity.
     * @param card Card instance used for the spawned card.
     * @param position Spawn position.
     */
    void createCard(const gameplay::CardInstance& card, const glm::vec2& position);

    /**
     * @brief Creates a visual slot on the board.
     * @param tag Readable slot label.
     * @param position Slot center position.
     * @param kind Logical slot role used for snap behavior.
     */
    cbit::ecs::GameObjectId createSlot(std::string_view tag, const glm::vec2& position, SlotKind kind);

    std::function<void()> _onReturnToMenu;
    gameplay::MatchState _matchState;
    float _deltaTimeSeconds = 0.0F;
    glm::vec2 _deckOrigin {0.0F, 0.0F};
    std::vector<cbit::ecs::GameObjectId> _deckCardIds;
    std::vector<cbit::ecs::GameObjectId> _handSlotIds;
    std::vector<cbit::ecs::GameObjectId> _headSlotIds;
    std::vector<DealStep> _dealSteps;
    std::size_t _nextDealStepIndex = 0;
    bool _isDealing = false;
    float _dealStepDelaySeconds = 0.08F;
    float _dealStepDelayRemainingSeconds = 0.0F;
    float _dealTravelDurationSeconds = 0.22F;
    float _dealArcHeight = 28.0F;
    ActiveDealAnimation _activeDealAnimation;
    SDL_AudioStream *_dealAudioStream = nullptr;
    std::vector<float> _dealSoundBuffer;
    cbit::ecs::GameObjectId _phaseTextId = 0;
    cbit::ecs::GameObjectId _roundTextId = 0;
    cbit::ecs::GameObjectId _localPlayerStatusTextId = 0;
    std::vector<cbit::ecs::GameObjectId> _opponentStatusTextIds;
};

} // namespace shark_card_game::scenes

#endif //SHARKCARDGAME_PLAY_SCENE_H
