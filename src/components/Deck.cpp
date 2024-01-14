//
// Created by User on 14/1/2024.
//

#include "Deck.h"

#include <algorithm>
#include <random>

void Deck::addCard(std::shared_ptr<Card> card) {
    mCards.push_back(card);
}

std::vector<std::shared_ptr<Card> > Deck::getCards() {
    return mCards;
}

void Deck::initialize() {
    // Add your card initialization logic here
    // For simplicity, let's assume cards have values from 1 to 13 and suits "Hearts", "Diamonds", "Clubs", "Spades"
    for (int value = 1; value <= 13; ++value) {
        for (const auto &suit: {"Hearts", "Diamonds", "Clubs", "Spades"}) {
            mCards.emplace_back(std::make_shared<Card>(value, suit));
        }
    }

    shuffle();
}

void Deck::shuffle() {
    std::shuffle(mCards.begin(), mCards.end(), std::mt19937(std::random_device()()));
}

std::shared_ptr<Card> Deck::giveCard() {
    auto card = mCards.back();
    return card;
}
