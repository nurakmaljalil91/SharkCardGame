//
// Created by User on 24/7/2024.
//

#include "PlayScene.h"

PlayScene::PlayScene() {

}

void PlayScene::cleanup() {
    Scene::cleanup();
}

void PlayScene::handleInput(SDL_Event event) {
    Scene::handleInput(event);
}

void PlayScene::render(SDL_Renderer *renderer) {
    Scene::render(renderer);
}

void PlayScene::update() {
    Scene::update();
}

void PlayScene::setup() {
    Scene::setup();
    _image = _registry.create();
    SDL_Texture *texture = AssetManager::getInstance().loadTexture("cardsSmall_tilemap_packed");
    _registry.emplace<TransformComponent>(_image, 100, 100, 100, 100);
    _registry.emplace<SpriteComponent>(_image, texture, SDL_Rect{0, 0, 100, 100}, SDL_Rect{100, 100, 100, 100});

}

PlayScene::~PlayScene() {

}
