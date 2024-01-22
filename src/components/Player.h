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

    void addCard(std::unique_ptr<Card> card);

    void showHand() const;

    int selectCard(int index);

private:
    std::string name_ = "Player";
    int score_ = 0;
    std::vector<std::unique_ptr<Card> > hand_;
};


#endif //PLAYER_H
