/**
 * @file components.h
 * @brief Header file for SharkCardGame-specific ECS components.
 *
 * This file contains the game-specific components used by SharkCardGame while
 * it is integrated with the current Cbit2D engine API.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-08-09
 */

#ifndef SHARKCARDGAME_COMPONENTS_H
#define SHARKCARDGAME_COMPONENTS_H

#include <cstdint>
#include <string>

#include <glm/vec2.hpp>

struct CardComponent {
    std::string name;
    int value;
    bool faceUp = false;
    std::uint64_t snappedSlotId = 0;
    glm::vec2 frontSourcePosition {0.0F, 0.0F};
};

enum class SlotKind {
    PlayerHand,
    PlayerHead,
    NonPlayableCharacterHand,
    NonPlayableCharacterHead
};

struct SlotComponent {
    SlotKind kind = SlotKind::PlayerHand;
};

#endif //SHARKCARDGAME_COMPONENTS_H
