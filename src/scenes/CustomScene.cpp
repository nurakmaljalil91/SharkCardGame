//
// Created by User on 21/7/2024.
//

#include "CustomScene.h"
#include <cbit2d/utilities/Logger.h>

CustomScene::CustomScene() {}

CustomScene::~CustomScene() {}

void CustomScene::setup(SDL_Renderer *renderer) {
    _image = _registry.create();
    _registry.emplace<TransformComponent>(_image, 0, 0, 100, 100);
    SDL_Texture *texture = _loadTexture(renderer, "resources/test.png");
    _registry.emplace<SpriteComponent>(_image, texture, SDL_Rect{0, 0, 100, 100}, SDL_Rect{100, 100, 100, 100});
}

void CustomScene::update() {
//    LOG_INFO("CustomScene updated");
}


void CustomScene::handleInput(SDL_Event event) {}

void CustomScene::cleanup() {}

