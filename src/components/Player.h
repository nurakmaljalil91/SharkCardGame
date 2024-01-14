//
// Created by User on 14/1/2024.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <memory>
#include <string>
#include <vector>

#include "Card.h"


class Player {
public:
    Player() = default;
    Player(std::string name, int score);
    ~Player() = default;

    void setName(std::string name);
    void setScore(int score);

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] int getScore() const;

    std::vector<std::shared_ptr<Card>> hand;

private:
    std::string mName = "Player";
    int mScore = 0;
};



#endif //PLAYER_H
