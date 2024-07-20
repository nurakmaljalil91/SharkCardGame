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


int main(int argc, char* argv[]) {
    // Initialize the logger
    Logger::init();

    LOG_INFO("Starting SDL2 application");

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR("SDL could not initialize! SDL_Error: {}", SDL_GetError());
        return 1;
    }
    LOG_INFO("SDL initialized successfully");

    // Create a window
    SDL_Window *window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        LOG_ERROR("Window could not be created! SDL_Error: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    LOG_INFO("Window created successfully");

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(SDL_GetRenderer(window), 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(SDL_GetRenderer(window));

        // Update screen
        SDL_RenderPresent(SDL_GetRenderer(window));
    }

    // Destroy window
    SDL_DestroyWindow(window);
    LOG_INFO("Window destroyed");

    // Quit SDL subsystems
    SDL_Quit();
    LOG_INFO("SDL quit");

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
