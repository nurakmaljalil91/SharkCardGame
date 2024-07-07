#include <iostream>
#include "entt/entt.hpp"
#include "random"
#include "components/Card.h"
#include "components/Deck.h"
#include "components/Player.h"
#include "SDL2/SDL.h"


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello SDL", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);

    SDL_Delay(2000);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

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
