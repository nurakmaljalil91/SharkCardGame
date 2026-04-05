/**
 * @file    match_rules.cpp
 * @brief   Source file for SharkCardGame round-rule helpers.
 * @details This file implements betting and payout helpers that operate on the
 *          plain gameplay state shared by SharkCardGame systems and scenes.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include "match_rules.h"

#include <algorithm>
#include <sstream>

namespace shark_card_game::gameplay {

/**
 * @brief Returns one player's visible scoring total.
 * @param player Player state to inspect.
 * @return Sum of hand and head card score values.
 */
int calculatePlayerTotal(const PlayerState& player)
{
    return (player.handCard ? player.handCard->definition.scoreValue : 0)
        + (player.headCard ? player.headCard->definition.scoreValue : 0);
}

/**
 * @brief Applies one bet choice and updates the round pot immediately.
 * @param matchState Match state to mutate.
 * @param seatIndex Seat that is acting.
 * @param amount Requested bet amount, or `0` to pass.
 * @return Actual committed amount after legal clamping.
 */
int applyBetChoice(MatchState& matchState, const int seatIndex, const int amount)
{
    if (seatIndex < 0 || seatIndex >= static_cast<int>(matchState.players.size())) {
        return 0;
    }

    auto& player = matchState.players[static_cast<std::size_t>(seatIndex)];
    const int committedBet = std::clamp(amount, 0, player.coins);

    player.coins -= committedBet;
    player.currentBet = committedBet;
    player.hasBetThisRound = true;
    player.declinedBet = committedBet == 0;
    matchState.round.pot += committedBet;

    return committedBet;
}

/**
 * @brief Resolves winners and applies round payouts.
 * @param matchState Match state to mutate.
 * @return Structured round-resolution result for UI presentation.
 */
RoundResolution resolveRound(MatchState& matchState)
{
    RoundResolution resolution;

    for (std::size_t playerIndex = 0; playerIndex < matchState.players.size(); ++playerIndex) {
        const int total = calculatePlayerTotal(matchState.players[playerIndex]);

        if (resolution.winningSeatIndices.empty() || total > resolution.winningTotal) {
            resolution.winningSeatIndices = {static_cast<int>(playerIndex)};
            resolution.winningTotal = total;
        } else if (total == resolution.winningTotal) {
            resolution.winningSeatIndices.push_back(static_cast<int>(playerIndex));
        }
    }

    const int winnerCount = static_cast<int>(resolution.winningSeatIndices.size());
    const int splitPayout = winnerCount > 0 ? matchState.round.pot / winnerCount : 0;
    for (const int winnerSeatIndex : resolution.winningSeatIndices) {
        matchState.players[static_cast<std::size_t>(winnerSeatIndex)].coins += splitPayout;
    }

    if (resolution.winningSeatIndices.size() == 1) {
        const int winnerSeatIndex = resolution.winningSeatIndices.front();
        resolution.summary = matchState.players[static_cast<std::size_t>(winnerSeatIndex)].displayName
            + " wins with " + std::to_string(resolution.winningTotal)
            + "  Pot: " + std::to_string(matchState.round.pot);
        return resolution;
    }

    std::ostringstream builder;
    builder << "Tie at " << resolution.winningTotal << " between ";
    for (std::size_t winnerIndex = 0; winnerIndex < resolution.winningSeatIndices.size(); ++winnerIndex) {
        if (winnerIndex > 0) {
            builder << ", ";
        }

        const int seatIndex = resolution.winningSeatIndices[winnerIndex];
        builder << matchState.players[static_cast<std::size_t>(seatIndex)].displayName;
    }

    builder << "  Split pot: " << splitPayout;
    resolution.summary = builder.str();

    return resolution;
}

} // namespace shark_card_game::gameplay
