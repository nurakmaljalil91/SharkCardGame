//
// Created by User on 14/1/2024.
//

#include "Player.h"

#include <iostream>

Player::Player(std::string name, const int score) : name_(std::move(name)), score_(score) {
}

void Player::setName(std::string name) {
    name_ = std::move(name);
}

void Player::setScore(const int score) {
    score_ = score;
}

std::string Player::getName() const { return name_; }

int Player::getScore() const { return score_; }

void Player::addCard(std::unique_ptr<Card> card) {
    hand_.push_back(std::move(card));
}

void Player::showHand() const {
    for (const auto &card: hand_) {
        std::cout << card->getName() << std::endl;
    }
}

int Player::selectCard(int index) {
    int score = hand_[index]->getValue();
    std::cout << name_ << " selected " << hand_[index]->getName() << std::endl;
    hand_.erase(hand_.begin() + index);

    return score;
}
