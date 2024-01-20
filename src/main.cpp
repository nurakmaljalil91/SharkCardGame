#include <iostream>
#include "entt/entt.hpp"
#include "random"
#include "components/Card.h"
#include "components/Deck.h"
#include "components/Player.h"

int main() {
    Deck deck;

    deck.initialize();

    deck.shuffle();

    Player player1("Player 1", 0);
    Player player2("Player 2", 0);

    player1.addCard(deck.drawCard());

    player2.addCard(deck.drawCard());

    std::cout << player1.getName() << " has " << player1.getScore() << " points" << std::endl;
    player1.showHand();

    std::cout << player2.getName() << " has " << player2.getScore() << " points" << std::endl;
    player2.showHand();
    return 0;
}
