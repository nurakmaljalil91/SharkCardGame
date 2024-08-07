/**
 * @file MenuScene.h
 * @brief Header file for the MenuScene class.
 *
 * This file contains the definition of the MenuScene class which is responsible for managing the menu scene in the game.
 * The MenuScene class is responsible for setting up the menu scene, updating the menu scene, and handling input for the menu scene.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-08-08
 */

#ifndef SHARKCARDGAME_MENUSCENE_H
#define SHARKCARDGAME_MENUSCENE_H

#include <cbit2d/core/Scene.h>

class MenuScene: public Scene {
public:
    ~MenuScene() override;

    void setup() override;

    void update(float deltaTime, Input &input) override;
};


#endif //SHARKCARDGAME_MENUSCENE_H
