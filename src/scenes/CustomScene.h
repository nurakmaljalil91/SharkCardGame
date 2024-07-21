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

    void init();
    void update();
    void render();
    void handleInput();
    void cleanup();

};


#endif //SHARKCARDGAME_CUSTOMSCENE_H
