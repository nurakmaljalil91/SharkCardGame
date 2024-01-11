#include <iostream>
#include "entt/entt.hpp"
#include "random"


struct Card {
    std::string name;
    int value{};
};


struct Player {
    std::string name;
    int score{};
    Card hand[2];
};

void initialize_deck(entt::registry &registry) {
    for (int i = 1; i <= 13; i++) {
        auto entity = registry.create();
        std::string name;

        switch (i) {
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
                name = std::to_string(i);
                break;
        }

        registry.emplace<Card>(entity, name, i);
    }
}

void initialize_players(entt::registry &registry) {
    auto player = registry.create();

    registry.emplace<Player>(player, "Player", 0);

    auto opponent = registry.create();
    registry.emplace<Player>(opponent, "Opponent", 0);
}

void deal_cards(entt::registry &registry) {
    auto deck = registry.view<Card>();
    auto players = registry.view<Player>();

    for (auto player: players) {
        auto &hand = registry.get<Player>(player).hand;

        for (int i = 0; i < 2; i++) {

            int random_card = rand() % deck.size();
            int j = 0;
            for (auto card: deck) {
                if (j == random_card) {
                    hand[i] = registry.get<Card>(card);
                    registry.destroy(card);
                }
                j++;
            }
        }
    }
}

void update_score(entt::registry &registry, entt::entity entity, int score) {
    auto &player = registry.get<Player>(entity);
    player.score += score;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    entt::registry registry;

    initialize_deck(registry);


    initialize_players(registry);

    deal_cards(registry);

    auto players = registry.view<Player>();
    for (auto player: players) {
        auto &hand = registry.get<Player>(player).hand;

        std::cout << registry.get<Player>(player).name << " has " << hand[0].name << " and " << hand[1].name
                  << std::endl;
    }

    std::cout << "Choose a card to play:" << std::endl;
    std::string chosen_card;
    std::cin >> chosen_card;

    auto deck = registry.view<Card>();

    for (auto card: deck) {
        if (registry.get<Card>(card).name == chosen_card) {
            std::cout << "You played " << registry.get<Card>(card).name << std::endl;
            update_score(registry, players[0], registry.get<Card>(card).value);
        }
    }

    return 0;
}
