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

#include <glm/vec2.hpp>

#include "cbit/core/scene.hpp"
#include "../gameplay/gameplay_state.h"
#include "../gameplay/match_rules.h"
#include "../components/components.h"
#include "play_scene_ui.h"

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
     * @brief Creates the placeholder betting panel for the upcoming betting phase.
     */
    void createBettingPanel();

    /**
     * @brief Creates the centered match-finished overlay panel.
     */
    void createMatchFinishedPanel();

    /**
     * @brief Refreshes betting panel text, styling, and button state.
     */
    void refreshBettingPanel();

    /**
     * @brief Refreshes the match-finished overlay visibility and text.
     */
    void refreshMatchFinishedPanel();

    /**
     * @brief Advances simple NPC betting turns.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void updateBettingPhase(float deltaTimeSeconds);

    /**
     * @brief Selects one local-player betting option.
     * @param amount Selected bet amount, or `-1` for pass.
     */
    void selectBetOption(int amount);

    /**
     * @brief Commits the local player's currently selected betting choice.
     */
    void confirmLocalPlayerBet();

    /**
     * @brief Applies one betting decision to the current active player.
     * @param amount Bet amount, or `0` to pass.
     */
    void commitBetForActivePlayer(int amount);

    /**
     * @brief Advances betting to the next player or phase.
     */
    void advanceBettingTurn();

    /**
     * @brief Advances reveal and round-resolution state.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void updateRevealAndResolution(float deltaTimeSeconds);

    /**
     * @brief Reveals all head cards for the current round.
     */
    void revealAllHeadCards();

    /**
     * @brief Computes winners and resolves round payouts.
     */
    void resolveRoundResult();

    /**
     * @brief Removes all round card entities from the scene.
     */
    void clearRoundCards();

    /**
     * @brief Starts the next round or finishes the match if all rounds are complete.
     */
    void advanceRoundFlow();

    /**
     * @brief Resets the current scene into a fresh new match.
     */
    void startNewGame();

    /**
     * @brief Reflows the remaining undealt cards into a compact deck grid.
     */
    void layoutRemainingDeck();

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
    std::vector<cbit::ecs::GameObjectId> _handCardIds;
    std::vector<cbit::ecs::GameObjectId> _headCardIds;
    std::vector<DealStep> _dealSteps;
    std::size_t _nextDealStepIndex = 0;
    bool _isDealing = false;
    float _dealStepDelaySeconds = 0.08F;
    float _dealStepDelayRemainingSeconds = 0.0F;
    float _dealTravelDurationSeconds = 0.22F;
    float _dealArcHeight = 28.0F;
    float _npcBetDelaySeconds = 0.7F;
    float _npcBetDelayRemainingSeconds = 0.0F;
    float _revealDelaySeconds = 1.0F;
    float _revealDelayRemainingSeconds = 0.0F;
    float _roundResolutionDelaySeconds = 2.0F;
    float _roundResolutionDelayRemainingSeconds = 0.0F;
    bool _roundResolved = false;
    int _winningTotal = 0;
    ActiveDealAnimation _activeDealAnimation;
    PlaySceneUi _ui;
    std::vector<float> _dealSoundBuffer;
    std::string _roundResultSummary;
    cbit::ecs::GameObjectId _phaseTextId = 0;
    cbit::ecs::GameObjectId _roundTextId = 0;
    cbit::ecs::GameObjectId _localPlayerStatusTextId = 0;
    cbit::ecs::GameObjectId _bettingPanelId = 0;
    cbit::ecs::GameObjectId _bettingSummaryTextId = 0;
    cbit::ecs::GameObjectId _betPassButtonId = 0;
    cbit::ecs::GameObjectId _betFiveButtonId = 0;
    cbit::ecs::GameObjectId _betTenButtonId = 0;
    cbit::ecs::GameObjectId _betTwentyButtonId = 0;
    cbit::ecs::GameObjectId _betConfirmButtonId = 0;
    cbit::ecs::GameObjectId _matchFinishedPanelId = 0;
    cbit::ecs::GameObjectId _matchFinishedSummaryTextId = 0;
    cbit::ecs::GameObjectId _newGameButtonId = 0;
    int _selectedBetAmount = 0;
    std::vector<int> _winningSeatIndices;
    std::vector<cbit::ecs::GameObjectId> _opponentStatusTextIds;
};

} // namespace shark_card_game::scenes

#endif //SHARKCARDGAME_PLAY_SCENE_H
