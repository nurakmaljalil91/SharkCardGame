/**
 * @file    gameplay_state.h
 * @brief   Header file for SharkCardGame gameplay state models.
 * @details This file defines the plain C++ gameplay structs used to represent
 *          match, round, player, and card state independently from ECS
 *          rendering concerns.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#ifndef SHARKCARDGAME_GAMEPLAY_STATE_H
#define SHARKCARDGAME_GAMEPLAY_STATE_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <glm/vec2.hpp>

namespace shark_card_game::gameplay {

/**
 * @brief Enumerates supported player control modes.
 */
enum class PlayerControlMode {
    Human,
    NonPlayableCharacter,
    Remote
};

/**
 * @brief Enumerates the high-level match phases.
 */
enum class MatchPhase {
    Deal,
    Betting,
    Reveal,
    RoundResolution,
    MatchFinished
};

/**
 * @brief Enumerates card suits.
 */
enum class CardSuit {
    Heart,
    Diamond,
    Club,
    Spade
};

/**
 * @brief Enumerates card ranks.
 */
enum class CardRank : int {
    Ace = 1,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King
};

/**
 * @brief Describes one unique card face.
 */
struct CardDefinition {
    CardSuit suit = CardSuit::Heart;
    CardRank rank = CardRank::Ace;
    std::string name;
    int scoreValue = 1;
    glm::vec2 sourcePosition {0.0F, 0.0F};
};

/**
 * @brief Describes one concrete card instance in a shuffled deck.
 */
struct CardInstance {
    std::uint32_t id = 0;
    CardDefinition definition;
};

/**
 * @brief Stores one player's match state.
 */
struct PlayerState {
    int seatIndex = 0;
    std::string displayName;
    PlayerControlMode controlMode = PlayerControlMode::Human;
    int coins = 100;
    int currentBet = 0;
    bool declinedBet = false;
    bool headCardRevealedToOwner = false;
    std::optional<CardInstance> handCard;
    std::optional<CardInstance> headCard;
};

/**
 * @brief Stores the current round state.
 */
struct RoundState {
    int roundNumber = 1;
    MatchPhase phase = MatchPhase::Deal;
    std::vector<CardInstance> shuffledDeck;
    std::size_t nextDrawIndex = 0;
};

/**
 * @brief Stores the full match state.
 */
struct MatchState {
    int playerCount = 2;
    int maxRounds = 4;
    int localPlayerSeatIndex = 0;
    std::vector<PlayerState> players;
    RoundState round;
};

/**
 * @brief Creates a standard 52-card deck definition list.
 * @return Vector of card definitions in deterministic base order.
 */
std::vector<CardDefinition> createStandardDeckDefinitions();

/**
 * @brief Creates the initial match state for a new game.
 * @param playerCount Number of players to include.
 * @param localPlayerSeatIndex Seat index controlled by the local player.
 * @return Initialized match state with shuffled deck and default players.
 */
MatchState createInitialMatchState(int playerCount, int localPlayerSeatIndex);

/**
 * @brief Returns a display label for one match phase.
 * @param phase Match phase to convert.
 * @return Human-readable phase label.
 */
const char* toString(MatchPhase phase);

} // namespace shark_card_game::gameplay

#endif //SHARKCARDGAME_GAMEPLAY_STATE_H
