/**
 * @file    table_layout.h
 * @brief   Header file for SharkCardGame table seat layout helpers.
 * @details This file defines seat layout data used by the play scene so table
 *          visuals can follow match seat indices without hard-coded player-0
 *          assumptions.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#ifndef SHARKCARDGAME_TABLE_LAYOUT_H
#define SHARKCARDGAME_TABLE_LAYOUT_H

#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "../components/components.h"
#include "../gameplay/gameplay_state.h"

namespace shark_card_game::scenes {

/**
 * @brief Stores one seat's visual layout on the table.
 */
struct SeatLayout {
    int playerIndex = 0;
    std::string handTag;
    std::string headTag;
    glm::vec2 handPosition {0.0F, 0.0F};
    glm::vec2 headPosition {0.0F, 0.0F};
    glm::vec2 labelPosition {0.0F, 0.0F};
    SlotKind handKind = SlotKind::PlayerHand;
    SlotKind headKind = SlotKind::PlayerHead;
};

/**
 * @brief Builds seat positions for the current match configuration.
 * @param matchState Match state to visualize.
 * @return Visual layout data keyed by player seat index.
 */
std::vector<SeatLayout> buildSeatLayouts(const gameplay::MatchState& matchState);

/**
 * @brief Builds a deterministic deal order for the visible table seats.
 * @param matchState Match state to inspect.
 * @return Seat indices ordered for dealing animation.
 */
std::vector<int> buildDealSeatOrder(const gameplay::MatchState& matchState);

} // namespace shark_card_game::scenes

#endif // SHARKCARDGAME_TABLE_LAYOUT_H
