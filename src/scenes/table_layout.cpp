/**
 * @file    table_layout.cpp
 * @brief   Source file for SharkCardGame table seat layout helpers.
 * @details This file maps logical player seats onto table positions for the
 *          current local-player perspective.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include "table_layout.h"

#include <array>
#include <stdexcept>

namespace shark_card_game::scenes {

namespace {

/**
 * @brief Stores one relative seat transform around the table.
 */
struct RelativeSeatLayout {
    glm::vec2 handPosition;
    glm::vec2 headPosition;
    glm::vec2 labelPosition;
    bool isLocal = false;
};

/**
 * @brief Returns the visual seat transforms for one player count.
 * @param playerCount Number of players in the current match.
 * @return Relative seat transforms where index zero is the local player.
 */
std::vector<RelativeSeatLayout> buildRelativeLayouts(const int playerCount)
{
    switch (playerCount) {
    case 2:
        return {
            {{590.0F, 614.0F}, {690.0F, 614.0F}, {640.0F, 674.0F}, true},
            {{590.0F, 176.0F}, {690.0F, 176.0F}, {640.0F, 124.0F}, false}
        };
    case 3:
        return {
            {{590.0F, 614.0F}, {690.0F, 614.0F}, {640.0F, 674.0F}, true},
            {{270.0F, 226.0F}, {370.0F, 226.0F}, {320.0F, 170.0F}, false},
            {{910.0F, 226.0F}, {1010.0F, 226.0F}, {960.0F, 170.0F}, false}
        };
    case 4:
        return {
            {{590.0F, 614.0F}, {690.0F, 614.0F}, {640.0F, 674.0F}, true},
            {{160.0F, 390.0F}, {260.0F, 390.0F}, {210.0F, 332.0F}, false},
            {{590.0F, 176.0F}, {690.0F, 176.0F}, {640.0F, 124.0F}, false},
            {{1020.0F, 390.0F}, {1120.0F, 390.0F}, {1070.0F, 332.0F}, false}
        };
    default:
        throw std::invalid_argument("SharkCardGame only supports 2 to 4 players");
    }
}

/**
 * @brief Builds a readable seat tag prefix.
 * @param relativeSeatIndex Relative seat index from the local perspective.
 * @param isLocal Whether the seat is the local player.
 * @return Stable prefix used for slot and label game object names.
 */
std::string buildSeatPrefix(const int relativeSeatIndex, const bool isLocal)
{
    if (isLocal) {
        return "LocalSeat";
    }

    return "Seat" + std::to_string(relativeSeatIndex);
}

} // namespace

/**
 * @brief Builds seat positions for the current match configuration.
 * @param matchState Match state to visualize.
 * @return Visual layout data keyed by player seat index.
 */
std::vector<SeatLayout> buildSeatLayouts(const gameplay::MatchState& matchState)
{
    const int playerCount = static_cast<int>(matchState.players.size());
    const int localSeatIndex = matchState.localPlayerSeatIndex;
    const std::vector<RelativeSeatLayout> relativeLayouts = buildRelativeLayouts(playerCount);

    std::vector<SeatLayout> layouts;
    layouts.reserve(matchState.players.size());

    for (int relativeSeatIndex = 0; relativeSeatIndex < playerCount; ++relativeSeatIndex) {
        const int playerIndex = (localSeatIndex + relativeSeatIndex) % playerCount;
        const RelativeSeatLayout& relativeLayout = relativeLayouts[static_cast<std::size_t>(relativeSeatIndex)];
        const std::string seatPrefix = buildSeatPrefix(relativeSeatIndex, relativeLayout.isLocal);

        layouts.push_back(SeatLayout{
            playerIndex,
            seatPrefix + "HandSlot",
            seatPrefix + "HeadSlot",
            relativeLayout.handPosition,
            relativeLayout.headPosition,
            relativeLayout.labelPosition,
            relativeLayout.isLocal ? SlotKind::PlayerHand : SlotKind::NonPlayableCharacterHand,
            relativeLayout.isLocal ? SlotKind::PlayerHead : SlotKind::NonPlayableCharacterHead
        });
    }

    return layouts;
}

/**
 * @brief Builds a deterministic deal order for the visible table seats.
 * @param matchState Match state to inspect.
 * @return Seat indices ordered for dealing animation.
 */
std::vector<int> buildDealSeatOrder(const gameplay::MatchState& matchState)
{
    const int playerCount = static_cast<int>(matchState.players.size());
    const int localSeatIndex = matchState.localPlayerSeatIndex;

    std::vector<int> order;
    order.reserve(matchState.players.size());

    for (int step = 1; step < playerCount; ++step) {
        order.push_back((localSeatIndex + step) % playerCount);
    }

    order.push_back(localSeatIndex);

    return order;
}

} // namespace shark_card_game::scenes
