//
// Created by User on 21/7/2024.
//

#ifndef SHARKCARDGAME_CUSTOMSCENE_H
#define SHARKCARDGAME_CUSTOMSCENE_H

#include <cbit2d/core/Scene.h>

class CustomScene: public Scene {
public:
    CustomScene();
    ~CustomScene();

    void setup(SDL_Renderer *renderer);
    void update();
    void handleInput(SDL_Event event);
    void cleanup();
private:
    entt::entity _image;
};


#endif //SHARKCARDGAME_CUSTOMSCENE_H
