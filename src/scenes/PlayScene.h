/**
 *  @file PlayScene.h
 *  @brief Header file for the PlayScene class.
 *
 *  This file contains the definition of the PlayScene class which is responsible for managing the play scene in the game.
 *  The PlayScene class is responsible for setting up the play scene, updating the play scene, and handling input for the play scene.
 *
 *  @author Nur Akmal bin Jalil
 *  @date 2024-07-21
 */

#ifndef SHARKCARDGAME_PLAYSCENE_H
#define SHARKCARDGAME_PLAYSCENE_H

#include <cbit2d/Cbit.h>
#include <glm/glm.hpp>
#include <algorithm> // For std::shuffle
#include <random>    // For std::random_device, std::mt19937
#include "../components/Components.h"
#include "../Global.h"

struct CardInfo {
    std::string name;
    int value;
    int spriteX;
    int spriteY;
    float positionX;
    float positionY;
};

class PlayScene : public Scene {

public:
    PlayScene();

    ~PlayScene() override;

    void setup() override;

    void update(float deltaTime, Input &input) override;

private:
    int const _cardWidth = 42;
    int const _cardHeight = 60;
    int const _margin = 10;
    glm::vec2 _initialPosition = {100, 100};

    // game logic
    int const _totalRounds = 3;
    int _currentRound = 1;

    GameObject _player;
    GameObject _playerHand;
    GameObject _playerHead;
    GameObject _npc;
    GameObject _npcHand;
    GameObject _npcHead;

    GameObject createCard(const CardInfo &cardInfo, float positionX, float positionY);
};


#endif //SHARKCARDGAME_PLAYSCENE_H
