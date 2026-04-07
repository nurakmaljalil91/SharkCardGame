/**
 * @file    round_flow_controller.cpp
 * @brief   Source file for SharkCardGame round-flow helpers.
 * @details This file implements phase timing and summary bookkeeping used by
 *          the SharkCardGame play scene.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-08
 */

#include "round_flow_controller.h"

namespace shark_card_game::scenes {

/**
 * @brief Resets phase timers and summary state at the start of a deal.
 * @param matchState Match state being prepared for a new round.
 */
void RoundFlowController::beginDeal(gameplay::MatchState& matchState)
{
    matchState.round.phase = gameplay::MatchPhase::Deal;
    matchState.round.pot = 0;
    matchState.round.playersActedCount = 0;
    matchState.round.activePlayerSeatIndex = matchState.localPlayerSeatIndex;

    _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
    _revealDelayRemainingSeconds = _revealDelaySeconds;
    _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
    _roundResolved = false;
    _winningTotal = 0;
    _winningSeatIndices.clear();
    _roundResultSummary.clear();
}

/**
 * @brief Marks the end of dealing and prepares the betting phase.
 * @param matchState Match state entering betting.
 */
void RoundFlowController::onDealFinished(gameplay::MatchState& matchState)
{
    matchState.round.phase = gameplay::MatchPhase::Betting;
    matchState.round.activePlayerSeatIndex = matchState.localPlayerSeatIndex;
    _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
}

/**
 * @brief Updates NPC betting timing and returns the next flow action.
 * @param matchState Match state to inspect.
 * @param deltaTimeSeconds Elapsed frame time in seconds.
 * @return Pending flow action for the play scene to apply.
 */
RoundFlowAction RoundFlowController::updateBetting(const gameplay::MatchState& matchState, const float deltaTimeSeconds)
{
    if (matchState.round.phase != gameplay::MatchPhase::Betting
        || matchState.round.activePlayerSeatIndex == matchState.localPlayerSeatIndex) {
        return {};
    }

    _npcBetDelayRemainingSeconds -= deltaTimeSeconds;
    if (_npcBetDelayRemainingSeconds > 0.0F) {
        return {};
    }

    const auto& activePlayer =
        matchState.players[static_cast<std::size_t>(matchState.round.activePlayerSeatIndex)];
    const int handValue = activePlayer.handCard ? activePlayer.handCard->definition.scoreValue : 0;

    int desiredBet = 0;
    if (handValue >= 11) {
        desiredBet = 20;
    } else if (handValue >= 7) {
        desiredBet = 10;
    } else if (handValue >= 4) {
        desiredBet = 5;
    }

    return {RoundFlowAction::Type::CommitBet, desiredBet};
}

/**
 * @brief Starts reveal timing after betting ends.
 * @param matchState Match state entering reveal.
 */
void RoundFlowController::beginReveal(gameplay::MatchState& matchState)
{
    matchState.round.phase = gameplay::MatchPhase::Reveal;
    matchState.round.activePlayerSeatIndex = matchState.localPlayerSeatIndex;
    _revealDelayRemainingSeconds = _revealDelaySeconds;
    _roundResolved = false;
}

/**
 * @brief Updates reveal or round-resolution timing.
 * @param matchState Match state to inspect.
 * @param deltaTimeSeconds Elapsed frame time in seconds.
 * @return Pending flow action for the play scene to apply.
 */
RoundFlowAction RoundFlowController::updateRevealAndResolution(
    const gameplay::MatchState& matchState,
    const float deltaTimeSeconds
)
{
    if (matchState.round.phase == gameplay::MatchPhase::Reveal && !_roundResolved) {
        _revealDelayRemainingSeconds -= deltaTimeSeconds;
        if (_revealDelayRemainingSeconds <= 0.0F) {
            return {RoundFlowAction::Type::ResolveRound};
        }

        return {};
    }

    if (matchState.round.phase == gameplay::MatchPhase::RoundResolution && _roundResolved) {
        _roundResolutionDelayRemainingSeconds -= deltaTimeSeconds;
        if (_roundResolutionDelayRemainingSeconds <= 0.0F) {
            return {RoundFlowAction::Type::AdvanceRound};
        }
    }

    return {};
}

/**
 * @brief Stores the most recent round-resolution result.
 * @param resolution Round resolution produced by gameplay rules.
 */
void RoundFlowController::applyRoundResolution(const gameplay::RoundResolution& resolution)
{
    _winningTotal = resolution.winningTotal;
    _winningSeatIndices = resolution.winningSeatIndices;
    _roundResultSummary = resolution.summary;
    _roundResolved = true;
    _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
}

/**
 * @brief Stores the final match summary when the match ends.
 * @param summary Final match summary text.
 */
void RoundFlowController::applyMatchFinishedSummary(std::string summary)
{
    _roundResultSummary = std::move(summary);
}

/**
 * @brief Clears transient state for a fresh match.
 */
void RoundFlowController::resetForNewMatch()
{
    _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
    _revealDelayRemainingSeconds = _revealDelaySeconds;
    _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
    _roundResolved = false;
    _winningTotal = 0;
    _winningSeatIndices.clear();
    _roundResultSummary.clear();
}

/**
 * @brief Returns whether the current round has been resolved.
 * @return `true` when winner data has been stored for this round.
 */
bool RoundFlowController::isRoundResolved() const
{
    return _roundResolved;
}

/**
 * @brief Returns the winning total for the current resolved round.
 * @return Winning hand total.
 */
int RoundFlowController::getWinningTotal() const
{
    return _winningTotal;
}

/**
 * @brief Returns winning seat indices for the current resolved round.
 * @return Winner seat indices.
 */
const std::vector<int>& RoundFlowController::getWinningSeatIndices() const
{
    return _winningSeatIndices;
}

/**
 * @brief Returns the latest round or match summary text.
 * @return Summary text for UI presentation.
 */
const std::string& RoundFlowController::getRoundResultSummary() const
{
    return _roundResultSummary;
}

} // namespace shark_card_game::scenes
