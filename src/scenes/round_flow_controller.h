/**
 * @file    round_flow_controller.h
 * @brief   Header file for SharkCardGame round-flow helpers.
 * @details This file defines a small controller that owns phase timers,
 *          NPC turn delays, and round-resolution presentation state.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-08
 */

#ifndef SHARKCARDGAME_ROUND_FLOW_CONTROLLER_H
#define SHARKCARDGAME_ROUND_FLOW_CONTROLLER_H

#include <string>
#include <vector>

#include "../gameplay/gameplay_state.h"
#include "../gameplay/match_rules.h"

namespace shark_card_game::scenes {

/**
 * @brief Describes one deferred action requested by the round-flow controller.
 */
struct RoundFlowAction {
    enum class Type {
        None,
        CommitBet,
        ResolveRound,
        AdvanceRound
    };

    Type type = Type::None;
    int amount = 0;
};

/**
 * @brief Owns timing and summary state for one running match.
 */
class RoundFlowController {
public:
    /**
     * @brief Resets phase timers and summary state at the start of a deal.
     * @param matchState Match state being prepared for a new round.
     */
    void beginDeal(gameplay::MatchState& matchState);

    /**
     * @brief Marks the end of dealing and prepares the betting phase.
     * @param matchState Match state entering betting.
     */
    void onDealFinished(gameplay::MatchState& matchState);

    /**
     * @brief Updates NPC betting timing and returns the next flow action.
     * @param matchState Match state to inspect.
     * @param deltaTimeSeconds Elapsed frame time in seconds.
     * @return Pending flow action for the play scene to apply.
     */
    RoundFlowAction updateBetting(const gameplay::MatchState& matchState, float deltaTimeSeconds);

    /**
     * @brief Starts reveal timing after betting ends.
     * @param matchState Match state entering reveal.
     */
    void beginReveal(gameplay::MatchState& matchState);

    /**
     * @brief Updates reveal or round-resolution timing.
     * @param matchState Match state to inspect.
     * @param deltaTimeSeconds Elapsed frame time in seconds.
     * @return Pending flow action for the play scene to apply.
     */
    RoundFlowAction updateRevealAndResolution(const gameplay::MatchState& matchState, float deltaTimeSeconds);

    /**
     * @brief Stores the most recent round-resolution result.
     * @param resolution Round resolution produced by gameplay rules.
     */
    void applyRoundResolution(const gameplay::RoundResolution& resolution);

    /**
     * @brief Stores the final match summary when the match ends.
     * @param summary Final match summary text.
     */
    void applyMatchFinishedSummary(std::string summary);

    /**
     * @brief Clears transient state for a fresh match.
     */
    void resetForNewMatch();

    /**
     * @brief Returns whether the current round has been resolved.
     * @return `true` when winner data has been stored for this round.
     */
    bool isRoundResolved() const;

    /**
     * @brief Returns the winning total for the current resolved round.
     * @return Winning hand total.
     */
    int getWinningTotal() const;

    /**
     * @brief Returns winning seat indices for the current resolved round.
     * @return Winner seat indices.
     */
    const std::vector<int>& getWinningSeatIndices() const;

    /**
     * @brief Returns the latest round or match summary text.
     * @return Summary text for UI presentation.
     */
    const std::string& getRoundResultSummary() const;

private:
    float _npcBetDelaySeconds = 0.7F;
    float _npcBetDelayRemainingSeconds = 0.0F;
    float _revealDelaySeconds = 1.0F;
    float _revealDelayRemainingSeconds = 0.0F;
    float _roundResolutionDelaySeconds = 2.0F;
    float _roundResolutionDelayRemainingSeconds = 0.0F;
    bool _roundResolved = false;
    int _winningTotal = 0;
    std::vector<int> _winningSeatIndices;
    std::string _roundResultSummary;
};

} // namespace shark_card_game::scenes

#endif // SHARKCARDGAME_ROUND_FLOW_CONTROLLER_H
