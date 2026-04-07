/**
 * @file    play_scene_ui.cpp
 * @brief   Source file for SharkCardGame play-scene UI helpers.
 * @details This file implements the persistent HUD and overlay entities used
 *          by the SharkCardGame play scene.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-07
 */

#include "play_scene_ui.h"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

#include <SDL3/SDL_pixels.h>
#include <glm/vec2.hpp>

#include "cbit/ecs/components.hpp"
#include "../gameplay/match_rules.h"

namespace shark_card_game::scenes {

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
void PlaySceneUi::create(
    cbit::ecs::EntityComponentSystem& world,
    const gameplay::MatchState& matchState,
    const std::function<void()>& onSelectPass,
    const std::function<void()>& onSelectBetFive,
    const std::function<void()>& onSelectBetTen,
    const std::function<void()>& onSelectBetTwenty,
    const std::function<void()>& onConfirmBet,
    const std::function<void()>& onStartNewGame
)
{
    auto phaseText = world.addGameObject("PhaseInfoText");
    phaseText.getComponent<cbit::ecs::TransformComponent>().position = {72.0F, 38.0F};
    auto& phaseTextComponent = phaseText.addComponent<cbit::ecs::TextComponent>();
    phaseTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    phaseTextComponent.fontSize = 20.0F;
    phaseTextComponent.color = {249, 214, 119, 255};
    phaseTextComponent.centered = false;
    _phaseTextId = phaseText.getComponent<cbit::ecs::IdComponent>().id;

    auto roundText = world.addGameObject("RoundInfoText");
    roundText.getComponent<cbit::ecs::TransformComponent>().position = {72.0F, 64.0F};
    auto& roundTextComponent = roundText.addComponent<cbit::ecs::TextComponent>();
    roundTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    roundTextComponent.fontSize = 16.0F;
    roundTextComponent.color = {214, 228, 240, 255};
    roundTextComponent.centered = false;
    _roundTextId = roundText.getComponent<cbit::ecs::IdComponent>().id;

    auto localPlayerStatus = world.addGameObject("LocalPlayerStatusText");
    localPlayerStatus.getComponent<cbit::ecs::TransformComponent>().position = {828.0F, 678.0F};
    auto& localPlayerStatusText = localPlayerStatus.addComponent<cbit::ecs::TextComponent>();
    localPlayerStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    localPlayerStatusText.fontSize = 16.0F;
    localPlayerStatusText.color = {235, 244, 255, 255};
    localPlayerStatusText.centered = false;
    _localPlayerStatusTextId = localPlayerStatus.getComponent<cbit::ecs::IdComponent>().id;

    _opponentStatusTextIds.assign(matchState.players.size(), 0);
    int opponentRow = 0;
    for (std::size_t playerIndex = 0; playerIndex < matchState.players.size(); ++playerIndex) {
        if (static_cast<int>(playerIndex) == matchState.localPlayerSeatIndex) {
            continue;
        }

        auto opponentStatus = world.addGameObject("OpponentStatusText" + std::to_string(playerIndex));
        opponentStatus.getComponent<cbit::ecs::TransformComponent>().position = {
            820.0F,
            168.0F + (static_cast<float>(opponentRow++) * 28.0F)
        };

        auto& opponentStatusText = opponentStatus.addComponent<cbit::ecs::TextComponent>();
        opponentStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        opponentStatusText.fontSize = 16.0F;
        opponentStatusText.color = {235, 244, 255, 255};
        opponentStatusText.centered = false;
        _opponentStatusTextIds[playerIndex] = opponentStatus.getComponent<cbit::ecs::IdComponent>().id;
    }

    constexpr glm::vec2 kPanelPosition{1032.0F, 603.0F};
    constexpr glm::vec2 kPanelSize{348.0F, 124.0F};
    constexpr float kButtonY = 641.0F;
    constexpr glm::vec2 kSmallButtonSize{56.0F, 42.0F};
    constexpr glm::vec2 kConfirmButtonSize{86.0F, 42.0F};

    auto panel = world.addGameObject("BettingPanel");
    panel.getComponent<cbit::ecs::TransformComponent>().position = kPanelPosition;
    _bettingPanelId = panel.getComponent<cbit::ecs::IdComponent>().id;

    auto& panelButton = panel.addComponent<cbit::ecs::ButtonComponent>();
    panelButton.size = kPanelSize;
    panelButton.backgroundColor = {16, 25, 40, 230};
    panelButton.hoverColor = {16, 25, 40, 230};
    panelButton.pressedColor = {16, 25, 40, 230};
    panelButton.borderColor = {86, 110, 145, 255};

    auto& panelText = panel.addComponent<cbit::ecs::TextComponent>();
    panelText.content = "Betting Phase";
    panelText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    panelText.fontSize = 20.0F;
    panelText.color = {249, 214, 119, 255};

    auto summary = world.addGameObject("BettingPanelSummary");
    summary.getComponent<cbit::ecs::TransformComponent>().position = {1032.0F, 581.0F};
    _bettingSummaryTextId = summary.getComponent<cbit::ecs::IdComponent>().id;

    auto& summaryText = summary.addComponent<cbit::ecs::TextComponent>();
    summaryText.content = "Your turn  Pot: 0  Coins: 100";
    summaryText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    summaryText.fontSize = 14.0F;
    summaryText.color = {214, 228, 240, 255};

    struct BettingButtonLayout {
        const char* name;
        const char* label;
        glm::vec2 position;
        glm::vec2 size;
        std::function<void()> onClick;
    };

    const std::vector<BettingButtonLayout> buttonLayouts{
        {"BetPassButton", "Pass", {892.0F, kButtonY}, kSmallButtonSize, onSelectPass},
        {"BetFiveButton", "5", {954.0F, kButtonY}, kSmallButtonSize, onSelectBetFive},
        {"BetTenButton", "10", {1016.0F, kButtonY}, kSmallButtonSize, onSelectBetTen},
        {"BetTwentyButton", "20", {1078.0F, kButtonY}, kSmallButtonSize, onSelectBetTwenty},
        {"BetConfirmButton", "Confirm", {1154.0F, kButtonY}, kConfirmButtonSize, onConfirmBet}
    };

    for (const BettingButtonLayout& buttonLayout : buttonLayouts) {
        auto button = world.addGameObject(buttonLayout.name);
        button.getComponent<cbit::ecs::TransformComponent>().position = buttonLayout.position;
        const auto buttonId = button.getComponent<cbit::ecs::IdComponent>().id;

        auto& buttonComponent = button.addComponent<cbit::ecs::ButtonComponent>();
        buttonComponent.size = buttonLayout.size;
        buttonComponent.backgroundColor = {34, 45, 67, 255};
        buttonComponent.hoverColor = {48, 64, 93, 255};
        buttonComponent.pressedColor = {22, 31, 46, 255};
        buttonComponent.borderColor = {231, 207, 115, 255};
        buttonComponent.onClick = buttonLayout.onClick;

        if (std::string_view(buttonLayout.name) == "BetPassButton") {
            _betPassButtonId = buttonId;
        } else if (std::string_view(buttonLayout.name) == "BetFiveButton") {
            _betFiveButtonId = buttonId;
        } else if (std::string_view(buttonLayout.name) == "BetTenButton") {
            _betTenButtonId = buttonId;
        } else if (std::string_view(buttonLayout.name) == "BetTwentyButton") {
            _betTwentyButtonId = buttonId;
        } else if (std::string_view(buttonLayout.name) == "BetConfirmButton") {
            _betConfirmButtonId = buttonId;
        }

        auto& buttonText = button.addComponent<cbit::ecs::TextComponent>();
        buttonText.content = buttonLayout.label;
        buttonText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        buttonText.fontSize = 16.0F;
        buttonText.color = {255, 255, 255, 255};
    }

    auto panelOverlay = world.addGameObject("MatchFinishedPanel");
    panelOverlay.getComponent<cbit::ecs::TransformComponent>().position = {640.0F, 360.0F};
    _matchFinishedPanelId = panelOverlay.getComponent<cbit::ecs::IdComponent>().id;

    auto& panelOverlayButton = panelOverlay.addComponent<cbit::ecs::ButtonComponent>();
    panelOverlayButton.size = {420.0F, 220.0F};
    panelOverlayButton.backgroundColor = {10, 16, 28, 0};
    panelOverlayButton.hoverColor = {10, 16, 28, 0};
    panelOverlayButton.pressedColor = {10, 16, 28, 0};
    panelOverlayButton.borderColor = {231, 207, 115, 0};

    auto& panelOverlayText = panelOverlay.addComponent<cbit::ecs::TextComponent>();
    panelOverlayText.content = " ";
    panelOverlayText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    panelOverlayText.fontSize = 28.0F;
    panelOverlayText.color = {249, 214, 119, 0};

    auto overlaySummary = world.addGameObject("MatchFinishedSummary");
    overlaySummary.getComponent<cbit::ecs::TransformComponent>().position = {640.0F, 360.0F};
    _matchFinishedSummaryTextId = overlaySummary.getComponent<cbit::ecs::IdComponent>().id;

    auto& overlaySummaryText = overlaySummary.addComponent<cbit::ecs::TextComponent>();
    overlaySummaryText.content = " ";
    overlaySummaryText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    overlaySummaryText.fontSize = 18.0F;
    overlaySummaryText.color = {235, 244, 255, 0};
    overlaySummaryText.wrapWidth = 340;

    auto newGameButton = world.addGameObject("NewGameButton");
    newGameButton.getComponent<cbit::ecs::TransformComponent>().position = {640.0F, 430.0F};
    _newGameButtonId = newGameButton.getComponent<cbit::ecs::IdComponent>().id;

    auto& newGameButtonComponent = newGameButton.addComponent<cbit::ecs::ButtonComponent>();
    newGameButtonComponent.size = {0.0F, 0.0F};
    newGameButtonComponent.backgroundColor = {34, 45, 67, 0};
    newGameButtonComponent.hoverColor = {48, 64, 93, 0};
    newGameButtonComponent.pressedColor = {22, 31, 46, 0};
    newGameButtonComponent.borderColor = {231, 207, 115, 0};
    newGameButtonComponent.onClick = onStartNewGame;

    auto& newGameButtonText = newGameButton.addComponent<cbit::ecs::TextComponent>();
    newGameButtonText.content = " ";
    newGameButtonText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    newGameButtonText.fontSize = 24.0F;
    newGameButtonText.color = {255, 255, 255, 0};
}

/**
 * @brief Refreshes UI text, colors, and visibility from the current model.
 * @param world Scene ECS world that owns the UI entities.
 * @param model Snapshot of play-scene state for UI presentation.
 */
void PlaySceneUi::refresh(cbit::ecs::EntityComponentSystem& world, const PlaySceneUiModel& model) const
{
    const bool bettingActive = !model.isDealing && model.matchState.round.phase == gameplay::MatchPhase::Betting;
    const bool localPlayersTurn = bettingActive
        && model.matchState.round.activePlayerSeatIndex == model.matchState.localPlayerSeatIndex;
    const int localSeatIndex = model.matchState.localPlayerSeatIndex;

    if (auto phaseText = world.getGameObject(_phaseTextId)) {
        auto& text = phaseText.getComponent<cbit::ecs::TextComponent>();
        text.content = std::string("Phase: ") + gameplay::toString(model.matchState.round.phase);
    }

    if (auto roundText = world.getGameObject(_roundTextId)) {
        auto& text = roundText.getComponent<cbit::ecs::TextComponent>();
        text.content = "Round: " + std::to_string(model.matchState.round.roundNumber)
            + "/" + std::to_string(model.matchState.maxRounds)
            + "  Players: " + std::to_string(model.matchState.playerCount)
            + "  Pot: " + std::to_string(model.matchState.round.pot)
            + "  Active: " + model.matchState.players[static_cast<std::size_t>(model.matchState.round.activePlayerSeatIndex)].displayName
            + "  Undealt Cards: " + std::to_string(
                model.matchState.round.shuffledDeck.size() - model.matchState.round.nextDrawIndex);
    }

    if (auto localPlayerStatus = world.getGameObject(_localPlayerStatusTextId)) {
        const auto& player = model.matchState.players[static_cast<std::size_t>(localSeatIndex)];
        std::ostringstream builder;
        builder << player.displayName
            << "  Coins: " << player.coins
            << "  Bet: " << player.currentBet
            << "  Hand: " << (player.handCard ? player.handCard->definition.name : "none")
            << "  Head: " << (player.headCardRevealedToOwner && player.headCard
                ? player.headCard->definition.name
                : "hidden");
        if (model.matchState.round.phase == gameplay::MatchPhase::Betting) {
            builder << "  Status: "
                << (model.matchState.round.activePlayerSeatIndex == localSeatIndex
                    ? "Your Turn"
                    : (player.hasBetThisRound ? (player.declinedBet ? "Passed" : "Locked") : "Waiting"));
        } else if (model.matchState.round.phase == gameplay::MatchPhase::RoundResolution) {
            builder << "  Total: " << gameplay::calculatePlayerTotal(player)
                << "  Status: "
                << (std::ranges::find(model.winningSeatIndices, localSeatIndex) != model.winningSeatIndices.end()
                    ? "Winner"
                    : "Lost");
        }
        localPlayerStatus.getComponent<cbit::ecs::TextComponent>().content = builder.str();
    }

    for (std::size_t playerIndex = 0; playerIndex < model.matchState.players.size(); ++playerIndex) {
        if (static_cast<int>(playerIndex) == localSeatIndex || playerIndex >= _opponentStatusTextIds.size()) {
            continue;
        }

        if (auto opponentStatus = world.getGameObject(_opponentStatusTextIds[playerIndex])) {
            const auto& player = model.matchState.players[playerIndex];
            std::ostringstream builder;
            builder << player.displayName
                << "  Coins: " << player.coins
                << "  Bet: " << player.currentBet
                << "  Hand: " << (player.handCard ? "hidden" : "none")
                << "  Head: " << (player.headCard ? player.headCard->definition.name : "none");
            if (model.matchState.round.phase == gameplay::MatchPhase::Betting) {
                builder << "  Status: "
                    << (model.matchState.round.activePlayerSeatIndex == static_cast<int>(playerIndex)
                        ? "Thinking"
                        : (player.hasBetThisRound ? (player.declinedBet ? "Passed" : "Locked") : "Waiting"));
            } else if (model.matchState.round.phase == gameplay::MatchPhase::RoundResolution) {
                builder << "  Total: " << gameplay::calculatePlayerTotal(player)
                    << "  Status: "
                    << (std::ranges::find(model.winningSeatIndices, static_cast<int>(playerIndex)) != model.winningSeatIndices.end()
                        ? "Winner"
                        : "Lost");
            }
            opponentStatus.getComponent<cbit::ecs::TextComponent>().content = builder.str();
        }
    }

    if (auto panel = world.getGameObject(_bettingPanelId)) {
        auto& button = panel.getComponent<cbit::ecs::ButtonComponent>();
        button.backgroundColor = bettingActive ? SDL_Color{16, 25, 40, 230} : SDL_Color{12, 18, 28, 180};
        button.hoverColor = button.backgroundColor;
        button.pressedColor = button.backgroundColor;
    }

    if (auto summary = world.getGameObject(_bettingSummaryTextId)) {
        auto& text = summary.getComponent<cbit::ecs::TextComponent>();
        if (model.isDealing) {
            text.content = "Dealing cards...";
        } else if (model.matchState.round.phase == gameplay::MatchPhase::Betting) {
            const auto& activePlayer =
                model.matchState.players[static_cast<std::size_t>(model.matchState.round.activePlayerSeatIndex)];
            const std::string selection = model.selectedBetAmount < 0
                ? "Pass"
                : (model.selectedBetAmount > 0 ? std::to_string(model.selectedBetAmount) : "None");
            text.content = (localPlayersTurn ? "Your turn" : activePlayer.displayName + " thinking")
                + std::string("  Pot: ") + std::to_string(model.matchState.round.pot)
                + "  Select: " + selection;
        } else if (model.matchState.round.phase == gameplay::MatchPhase::Reveal) {
            text.content = "Revealing head cards...  Pot: " + std::to_string(model.matchState.round.pot);
        } else if (model.matchState.round.phase == gameplay::MatchPhase::RoundResolution) {
            text.content = model.roundResultSummary.empty() ? "Resolving round..." : model.roundResultSummary;
        } else {
            text.content = "Waiting for betting phase";
        }
    }

    const auto updateBetButton = [&](const cbit::ecs::GameObjectId buttonId, const int amount) {
        if (auto buttonObject = world.getGameObject(buttonId)) {
            auto& button = buttonObject.getComponent<cbit::ecs::ButtonComponent>();
            const bool isSelected = model.selectedBetAmount == amount;
            const SDL_Color baseColor = localPlayersTurn ? SDL_Color{34, 45, 67, 255} : SDL_Color{24, 31, 46, 255};
            const SDL_Color selectedColor{93, 71, 28, 255};
            button.backgroundColor = isSelected ? selectedColor : baseColor;
            button.hoverColor = isSelected ? SDL_Color{114, 87, 34, 255} : SDL_Color{48, 64, 93, 255};
            button.pressedColor = isSelected ? SDL_Color{80, 61, 24, 255} : SDL_Color{22, 31, 46, 255};
            button.borderColor = localPlayersTurn ? SDL_Color{231, 207, 115, 255} : SDL_Color{92, 101, 118, 255};
            if (!localPlayersTurn && amount != 0) {
                button.hoverColor = baseColor;
                button.pressedColor = baseColor;
            }
        }
    };

    updateBetButton(_betPassButtonId, -1);
    updateBetButton(_betFiveButtonId, 5);
    updateBetButton(_betTenButtonId, 10);
    updateBetButton(_betTwentyButtonId, 20);

    if (auto confirmButton = world.getGameObject(_betConfirmButtonId)) {
        auto& button = confirmButton.getComponent<cbit::ecs::ButtonComponent>();
        const bool canConfirm = localPlayersTurn && model.selectedBetAmount != 0;
        button.backgroundColor = canConfirm ? SDL_Color{34, 45, 67, 255} : SDL_Color{24, 31, 46, 255};
        button.hoverColor = canConfirm ? SDL_Color{48, 64, 93, 255} : SDL_Color{24, 31, 46, 255};
        button.pressedColor = canConfirm ? SDL_Color{22, 31, 46, 255} : SDL_Color{24, 31, 46, 255};
        button.borderColor = canConfirm ? SDL_Color{231, 207, 115, 255} : SDL_Color{92, 101, 118, 255};
    }

    const bool showMatchFinished = model.matchState.round.phase == gameplay::MatchPhase::MatchFinished;
    if (auto overlay = world.getGameObject(_matchFinishedPanelId)) {
        auto& button = overlay.getComponent<cbit::ecs::ButtonComponent>();
        auto& text = overlay.getComponent<cbit::ecs::TextComponent>();
        button.backgroundColor = showMatchFinished ? SDL_Color{10, 16, 28, 230} : SDL_Color{10, 16, 28, 0};
        button.hoverColor = button.backgroundColor;
        button.pressedColor = button.backgroundColor;
        button.borderColor = showMatchFinished ? SDL_Color{231, 207, 115, 255} : SDL_Color{231, 207, 115, 0};
        text.content = showMatchFinished ? "Match Finished" : " ";
        text.color = showMatchFinished ? SDL_Color{249, 214, 119, 255} : SDL_Color{249, 214, 119, 0};
    }

    if (auto summary = world.getGameObject(_matchFinishedSummaryTextId)) {
        auto& text = summary.getComponent<cbit::ecs::TextComponent>();
        text.content = showMatchFinished ? model.roundResultSummary : " ";
        text.color = showMatchFinished ? SDL_Color{235, 244, 255, 255} : SDL_Color{235, 244, 255, 0};
    }

    if (auto newGameButton = world.getGameObject(_newGameButtonId)) {
        auto& button = newGameButton.getComponent<cbit::ecs::ButtonComponent>();
        auto& text = newGameButton.getComponent<cbit::ecs::TextComponent>();
        button.size = showMatchFinished ? glm::vec2{180.0F, 56.0F} : glm::vec2{0.0F, 0.0F};
        button.backgroundColor = showMatchFinished ? SDL_Color{34, 45, 67, 255} : SDL_Color{34, 45, 67, 0};
        button.hoverColor = showMatchFinished ? SDL_Color{48, 64, 93, 255} : SDL_Color{48, 64, 93, 0};
        button.pressedColor = showMatchFinished ? SDL_Color{22, 31, 46, 255} : SDL_Color{22, 31, 46, 0};
        button.borderColor = showMatchFinished ? SDL_Color{231, 207, 115, 255} : SDL_Color{231, 207, 115, 0};
        text.content = showMatchFinished ? "New Game" : " ";
        text.color = showMatchFinished ? SDL_Color{255, 255, 255, 255} : SDL_Color{255, 255, 255, 0};
    }
}

} // namespace shark_card_game::scenes
