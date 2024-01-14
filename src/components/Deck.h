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

    void addCard(std::shared_ptr<Card> card);

    [[nodiscard]] std::vector<std::shared_ptr<Card> > getCards();

    void initialize();

    std::shared_ptr<Card> giveCard();

private:
    void shuffle();

    std::vector<std::shared_ptr<Card> > mCards;
};


#endif //DECK_H
