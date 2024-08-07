/**
 * @file PlayScene.cpp
 * @brief Implementation file for the PlayScene class.
 *
 * This file contains the implementation of the PlayScene class which is responsible for managing the play scene in the game.
 * The PlayScene class is responsible for setting up the play scene, updating the play scene, and handling input for the play scene.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-21
 */

#include "PlayScene.h"

PlayScene::PlayScene() :
        _image(entt::null),
        Scene() {}

PlayScene::~PlayScene() = default;

void PlayScene::setup() {
//    Scene::setup();
    LOG_INFO("PlayScene setup");
    toggleDebug();
    _image = _ecs.registry.create();
    _ecs.registry.emplace<TransformComponent>(_image, 100, 100, 224, 96);
    _ecs.registry.emplace<SpriteComponent>(_image, "cardsSmall_tilemap_packed", 0, 0, 224, 96);
}

void PlayScene::update(float deltaTime, Input &input) {
    Scene::update(deltaTime, input);
}




