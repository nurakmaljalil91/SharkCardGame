//
// Created by User on 14/1/2024.
//

#include "Deck.h"

#include <algorithm>
#include <random>


void Deck::initialize() {
    // Add your card initialization logic here
    // For simplicity, let's assume cards have values from 1 to 13 and suits "Hearts", "Diamonds", "Clubs", "Spades"
    for (int value = 1; value <= 13; ++value) {
        for (const auto &suit: {"Hearts", "Diamonds", "Clubs", "Spades"}) {
            cards_.emplace_back(std::make_unique<Card>(value, suit));
        }
    }
}

void Deck::shuffle() {
    std::shuffle(cards_.begin(), cards_.end(), std::mt19937(std::random_device()()));
}

std::unique_ptr<Card> Deck::drawCard() {
    if (cards_.empty()) {
        return nullptr;
    }
    std::unique_ptr<Card> card = std::move(cards_.back());
    cards_.pop_back();
    return card;
}
