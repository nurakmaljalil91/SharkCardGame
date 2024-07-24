//
// Created by User on 24/7/2024.
//

#ifndef SHARKCARDGAME_PLAYSCENE_H
#define SHARKCARDGAME_PLAYSCENE_H


#include <cbit2d/core/Scene.h>
#include <cbit2d/core/AssetManager.h>

class PlayScene : public Scene {

public:
    PlayScene();

    ~PlayScene() override;

    void setup() override;

    void update() override;

    void render(SDL_Renderer *renderer) override;

    void handleInput(SDL_Event event) override;

    void cleanup() override;

private:
    entt::entity _image;
};


#endif //SHARKCARDGAME_PLAYSCENE_H
