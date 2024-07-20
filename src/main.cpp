/**
 * @file main.cpp
 * @brief Main file for the SDL2 application with logging using spdlog.
 *
 * This file contains the main function for an SDL2 application that sets up
 * a window and logs events using the Logger class defined in Logger.h.
 *
 * The application initializes SDL2, creates a window, and enters a main loop
 * where it handles events until the user requests to quit.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-20
 */

#include <iostream>
#include "entt/entt.hpp"
#include "random"
#include "components/Card.h"
#include "components/Deck.h"
#include "components/Player.h"
#include "SDL2/SDL.h"
#include "utilities/Logger.h"
#include "core/Application.h"


int main(int argc, char* argv[]) {
    Application app;
    if (!app.init()) {
        return 1;
    }
    app.run();
    return 0;
}

void SetupCardGame() {
    Deck deck;

    deck.initialize();

    deck.shuffle();

    Player player1("Player 1", 0);
    Player player2("Player 2", 0);

    player1.addCard(deck.drawCard());
    player1.addCard(deck.drawCard());

    player2.addCard(deck.drawCard());
    player2.addCard(deck.drawCard());

    std::cout << player1.getName() << " has " << player1.getScore() << " points" << std::endl;
    player1.showHand();

    std::cout << player2.getName() << " has " << player2.getScore() << " points" << std::endl;
    player2.showHand();

    std::cout << "Select a card to play: enter 0 or 1" << std::endl;

    int index;
    std::cin >> index;

    auto score1 = player1.selectCard(index);

    // generate random number between 0 and 1
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 1);
    auto index2 = distr(gen);

    auto score2 = player2.selectCard(index2);

    if (score1 > score2) {
        player1.setScore(player1.getScore() + score1 + score2);
    } else {
        player2.setScore(player2.getScore() + score1 + score2);
    }

    std::cout << player1.getName() << " has " << player1.getScore() << " points" << std::endl;
    std::cout << player2.getName() << " has " << player2.getScore() << " points" << std::endl;
}
