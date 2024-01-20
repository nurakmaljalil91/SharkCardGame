//
// Created by User on 14/1/2024.
//

#include "Card.h"

Card::Card(const int value, std::string suit) : value_(value), suit_(std::move(suit)) {
}

Card::~Card() = default;

std::string Card::getName() const {
    std::string name;
    switch (value_) {
        case 1:
            name = "Ace";
            break;
        case 11:
            name = "Jack";
            break;
        case 12:
            name = "Queen";
            break;
        case 13:
            name = "King";
            break;
        default:
            name = std::to_string(value_);
            break;
    }

    return name + " of " + suit_;
}

