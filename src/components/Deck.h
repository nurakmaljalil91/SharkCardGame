//
// Created by User on 14/1/2024.
//

#ifndef DECK_H
#define DECK_H
#include <memory>
#include <vector>

#include "Card.h"


class Deck {
public:
    Deck() = default;

    ~Deck() = default;

    void initialize();

    void shuffle();

    std::unique_ptr<Card> drawCard();

private:
    std::vector<std::unique_ptr<Card> > cards_;
};


#endif //DECK_H
