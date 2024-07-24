//
// Created by User on 21/7/2024.
//

#ifndef SHARKCARDGAME_CUSTOMSCENE_H
#define SHARKCARDGAME_CUSTOMSCENE_H

#include <cbit2d/core/Scene.h>
#include <cbit2d/core/AssetManager.h>

class CustomScene: public Scene {
public:
    CustomScene();
    ~CustomScene() override;

    void setup() override;
    void update() override;
    void render(SDL_Renderer *renderer) override;
    void handleInput(SDL_Event event) override;
    void cleanup() override;
private:
    entt::entity _image;
};


#endif //SHARKCARDGAME_CUSTOMSCENE_H
