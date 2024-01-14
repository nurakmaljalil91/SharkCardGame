//
// Created by User on 14/1/2024.
//

#include "Player.h"

Player::Player(std::string name, const int score) : mName(std::move(name)), mScore(score) {
}

void Player::setName(std::string name) {
    mName = std::move(name);
}

void Player::setScore(const int score) {
    mScore = score;
}

std::string Player::getName() const { return mName; }

int Player::getScore() const { return mScore; }
