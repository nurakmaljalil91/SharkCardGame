/**
 * @file    play_scene_ui.h
 * @brief   Header file for SharkCardGame play-scene UI helpers.
 * @details This file defines a focused UI helper that owns the HUD, betting
 *          panel, and match-finished overlay for the play scene.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-07
 */

#ifndef SHARKCARDGAME_PLAY_SCENE_UI_H
#define SHARKCARDGAME_PLAY_SCENE_UI_H

#include <functional>
#include <string>
#include <vector>

#include "cbit/ecs/entity_component_system.hpp"
#include "../gameplay/gameplay_state.h"

namespace shark_card_game::scenes {

/**
 * @brief Snapshot of play-scene state needed to refresh the UI.
 */
struct PlaySceneUiModel {
    const gameplay::MatchState& matchState;
    bool isDealing = false;
    int selectedBetAmount = 0;
    const std::vector<int>& winningSeatIndices;
    const std::string& roundResultSummary;
};

/**
 * @brief Owns the ECS entities used by the play-scene UI.
 */
class PlaySceneUi {
public:
    /**
     * @brief Creates the persistent play-scene UI entities.
     * @param world Scene ECS world that owns the UI entities.
     * @param matchState Match state used to size seat-specific status text.
     * @param onSelectPass Callback for the pass betting option.
     * @param onSelectBetFive Callback for the 5-coin betting option.
     * @param onSelectBetTen Callback for the 10-coin betting option.
     * @param onSelectBetTwenty Callback for the 20-coin betting option.
     * @param onConfirmBet Callback that commits the selected bet.
     * @param onStartNewGame Callback that resets the match.
     */
    void create(
        cbit::ecs::EntityComponentSystem& world,
        const gameplay::MatchState& matchState,
        const std::function<void()>& onSelectPass,
        const std::function<void()>& onSelectBetFive,
        const std::function<void()>& onSelectBetTen,
        const std::function<void()>& onSelectBetTwenty,
        const std::function<void()>& onConfirmBet,
        const std::function<void()>& onStartNewGame
    );

    /**
     * @brief Refreshes UI text, colors, and visibility from the current model.
     * @param world Scene ECS world that owns the UI entities.
     * @param model Snapshot of play-scene state for UI presentation.
     */
    void refresh(cbit::ecs::EntityComponentSystem& world, const PlaySceneUiModel& model) const;

private:
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
    std::vector<cbit::ecs::GameObjectId> _opponentStatusTextIds;
};

} // namespace shark_card_game::scenes

#endif // SHARKCARDGAME_PLAY_SCENE_UI_H
