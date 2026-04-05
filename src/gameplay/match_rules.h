/**
 * @file    match_rules.h
 * @brief   Header file for SharkCardGame round-rule helpers.
 * @details This file defines helper functions that apply betting and payout
 *          rules to the plain gameplay state without depending on scene code.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#ifndef SHARKCARDGAME_MATCH_RULES_H
#define SHARKCARDGAME_MATCH_RULES_H

#include <string>
#include <vector>

#include "gameplay_state.h"

namespace shark_card_game::gameplay {

/**
 * @brief Stores the result of one round resolution.
 */
struct RoundResolution {
    int winningTotal = 0;
    std::vector<int> winningSeatIndices;
    std::string summary;
};

/**
 * @brief Returns one player's visible scoring total.
 * @param player Player state to inspect.
 * @return Sum of hand and head card score values.
 */
int calculatePlayerTotal(const PlayerState& player);

/**
 * @brief Applies one bet choice and updates the round pot immediately.
 * @param matchState Match state to mutate.
 * @param seatIndex Seat that is acting.
 * @param amount Requested bet amount, or `0` to pass.
 * @return Actual committed amount after legal clamping.
 */
int applyBetChoice(MatchState& matchState, int seatIndex, int amount);

/**
 * @brief Resolves winners and applies round payouts.
 * @param matchState Match state to mutate.
 * @return Structured round-resolution result for UI presentation.
 */
RoundResolution resolveRound(MatchState& matchState);

} // namespace shark_card_game::gameplay

#endif // SHARKCARDGAME_MATCH_RULES_H
