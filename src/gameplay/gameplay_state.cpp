/**
 * @file    gameplay_state.cpp
 * @brief   Source file for SharkCardGame gameplay state helpers.
 * @details This file defines the helper functions that create the initial
 *          gameplay state used by SharkCardGame.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include "gameplay_state.h"

#include <algorithm>
#include <array>
#include <random>
#include <utility>

namespace shark_card_game::gameplay {

namespace {

/**
 * @brief Returns the display name of one suit.
 * @param suit Suit to convert.
 * @return Human-readable suit name.
 */
const char* toString(const CardSuit suit)
{
    switch (suit) {
    case CardSuit::Heart:
        return "Heart";
    case CardSuit::Diamond:
        return "Diamond";
    case CardSuit::Club:
        return "Club";
    case CardSuit::Spade:
        return "Spade";
    }

    return "Unknown";
}

/**
 * @brief Returns the display name of one rank.
 * @param rank Rank to convert.
 * @return Human-readable rank name.
 */
const char* toString(const CardRank rank)
{
    switch (rank) {
    case CardRank::Ace:
        return "Ace";
    case CardRank::Two:
        return "Two";
    case CardRank::Three:
        return "Three";
    case CardRank::Four:
        return "Four";
    case CardRank::Five:
        return "Five";
    case CardRank::Six:
        return "Six";
    case CardRank::Seven:
        return "Seven";
    case CardRank::Eight:
        return "Eight";
    case CardRank::Nine:
        return "Nine";
    case CardRank::Ten:
        return "Ten";
    case CardRank::Jack:
        return "Jack";
    case CardRank::Queen:
        return "Queen";
    case CardRank::King:
        return "King";
    }

    return "Unknown";
}

/**
 * @brief Returns the tilemap source position for one card face.
 * @param suit Card suit.
 * @param rank Card rank.
 * @return Source position inside the cards tilemap.
 */
glm::vec2 buildSourcePosition(const CardSuit suit, const CardRank rank)
{
    const int column = static_cast<int>(rank) - 1;

    int row = 0;
    switch (suit) {
    case CardSuit::Heart:
        row = 0;
        break;
    case CardSuit::Diamond:
        row = 1;
        break;
    case CardSuit::Club:
        row = 2;
        break;
    case CardSuit::Spade:
        row = 3;
        break;
    }

    return {
        static_cast<float>(column * 42),
        static_cast<float>(row * 60)
    };
}

/**
 * @brief Builds one display name from rank and suit.
 * @param rank Card rank.
 * @param suit Card suit.
 * @return Full display name for the card.
 */
std::string buildCardName(const CardRank rank, const CardSuit suit)
{
    return std::string(toString(rank)) + " of " + toString(suit);
}

/**
 * @brief Returns the default player label for one seat.
 * @param seatIndex Seat index in the match.
 * @param controlMode Control mode assigned to the seat.
 * @return Human-readable player label.
 */
std::string buildPlayerName(const int seatIndex, const PlayerControlMode controlMode)
{
    if (controlMode == PlayerControlMode::Human) {
        return "Player";
    }

    return "NPC " + std::to_string(seatIndex);
}

} // namespace

/**
 * @brief Creates a standard 52-card deck definition list.
 * @return Vector of card definitions in deterministic base order.
 */
std::vector<CardDefinition> createStandardDeckDefinitions()
{
    constexpr std::array<CardSuit, 4> suits {
        CardSuit::Heart,
        CardSuit::Diamond,
        CardSuit::Club,
        CardSuit::Spade
    };

    constexpr std::array<CardRank, 13> ranks {
        CardRank::Ace,
        CardRank::Two,
        CardRank::Three,
        CardRank::Four,
        CardRank::Five,
        CardRank::Six,
        CardRank::Seven,
        CardRank::Eight,
        CardRank::Nine,
        CardRank::Ten,
        CardRank::Jack,
        CardRank::Queen,
        CardRank::King
    };

    std::vector<CardDefinition> definitions;
    definitions.reserve(suits.size() * ranks.size());

    for (const CardSuit suit : suits) {
        for (const CardRank rank : ranks) {
            definitions.push_back(CardDefinition {
                suit,
                rank,
                buildCardName(rank, suit),
                static_cast<int>(rank),
                buildSourcePosition(suit, rank)
            });
        }
    }

    return definitions;
}

/**
 * @brief Creates a freshly shuffled round state.
 * @param roundNumber Round number to assign.
 * @return Initialized round state with a shuffled deck.
 */
RoundState createShuffledRound(const int roundNumber)
{
    RoundState roundState;
    roundState.roundNumber = std::max(1, roundNumber);
    roundState.phase = MatchPhase::Deal;
    roundState.pot = 0;
    roundState.activePlayerSeatIndex = 0;
    roundState.playersActedCount = 0;

    std::vector<CardDefinition> definitions = createStandardDeckDefinitions();
    roundState.shuffledDeck.reserve(definitions.size());
    for (std::size_t cardIndex = 0; cardIndex < definitions.size(); ++cardIndex) {
        roundState.shuffledDeck.push_back(CardInstance {
            static_cast<std::uint32_t>(cardIndex + 1),
            std::move(definitions[cardIndex])
        });
    }

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::shuffle(roundState.shuffledDeck.begin(), roundState.shuffledDeck.end(), generator);

    return roundState;
}

/**
 * @brief Creates the initial match state for a new game.
 * @param playerCount Number of players to include.
 * @param localPlayerSeatIndex Seat index controlled by the local player.
 * @return Initialized match state with shuffled deck and default players.
 */
MatchState createInitialMatchState(int playerCount, int localPlayerSeatIndex)
{
    MatchState matchState;
    matchState.playerCount = std::clamp(playerCount, 2, 4);
    matchState.localPlayerSeatIndex = std::clamp(localPlayerSeatIndex, 0, matchState.playerCount - 1);

    matchState.players.reserve(static_cast<std::size_t>(matchState.playerCount));
    for (int seatIndex = 0; seatIndex < matchState.playerCount; ++seatIndex) {
        const PlayerControlMode controlMode = seatIndex == matchState.localPlayerSeatIndex
            ? PlayerControlMode::Human
            : PlayerControlMode::NonPlayableCharacter;

        matchState.players.push_back(PlayerState {
            seatIndex,
            buildPlayerName(seatIndex, controlMode),
            controlMode
        });
    }

    matchState.round = createShuffledRound(1);

    return matchState;
}

/**
 * @brief Returns a display label for one match phase.
 * @param phase Match phase to convert.
 * @return Human-readable phase label.
 */
const char* toString(const MatchPhase phase)
{
    switch (phase) {
    case MatchPhase::Deal:
        return "Deal";
    case MatchPhase::Betting:
        return "Betting";
    case MatchPhase::Reveal:
        return "Reveal";
    case MatchPhase::RoundResolution:
        return "Round Resolution";
    case MatchPhase::MatchFinished:
        return "Match Finished";
    }

    return "Unknown";
}

} // namespace shark_card_game::gameplay
