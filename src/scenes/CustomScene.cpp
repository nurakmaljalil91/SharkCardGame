//
// Created by User on 21/7/2024.
//

#include "CustomScene.h"
#include <cbit2d/utilities/Logger.h>

CustomScene::CustomScene() {}

CustomScene::~CustomScene() {}

void CustomScene::setup() {
    LOG_INFO("CustomScene setup");
    _image = _registry.create();
    _registry.emplace<TransformComponent>(_image, 0, 0, 400, 400);
    SDL_Texture *texture = AssetManager::getInstance().loadTexture("cardsSmall_tilemap");
    _registry.emplace<SpriteComponent>(_image, texture, SDL_Rect{0, 0, 400, 400}, SDL_Rect{100, 100, 400, 400});
}

void CustomScene::update() {
    LOG_INFO("CustomScene updated");
    Scene::update();
}


void CustomScene::handleInput(SDL_Event event) {
    Scene::handleInput(event);
    LOG_INFO("CustomScene handled input");
}

void CustomScene::cleanup() {}

void CustomScene::render(SDL_Renderer *renderer) {
    Scene::render(renderer);
    LOG_INFO("CustomScene rendered");
}

