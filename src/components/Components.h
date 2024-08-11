/**
 * @file GameComponent.h
 * @brief Header file for the GameComponent class.
 *
 * This file contains the definition of the GameComponent class which is responsible for managing the game components in the game.
 * The GameComponent class is responsible for setting up the game components, updating the game components, and handling input for the game components.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-08-09
 */

#ifndef SHARKCARDGAME_COMPONENTS_H
#define SHARKCARDGAME_COMPONENTS_H

#include <string>
#include <vector>

struct CardComponent {
    std::string name;
    int value;
};

struct PlayerComponent {
    std::string name;
    int money = 1000;
    std::vector<CardComponent> hand;
    std::vector<CardComponent> head;
};

struct NonPlayableCharacterComponent {
    std::string name;
    int money = 1000;
    std::vector<CardComponent> hand;
    std::vector<CardComponent> head;
};


#endif //SHARKCARDGAME_COMPONENTS_H
