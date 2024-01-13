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
    std::vector<Card> hand;
};

void initialize_deck(entt::registry &registry) {
    for (int i = 1; i <= 13; i++) {
        const auto entity = registry.create();
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
    const auto player = registry.create();

    registry.emplace<Player>(player, "Player", 0);

    const auto opponent = registry.create();
    registry.emplace<Player>(opponent, "Opponent", 0);
}

void deal_cards(entt::registry &registry) {
    const auto deck = registry.view<Card>();
    const auto players = registry.view<Player>();

    if (deck.size() < 2 * players.size()) {
        std::cout << "Not enough cards in the deck!" << std::endl;
        return;
    }

    for (const auto player: players) {
        auto &hand = registry.get<Player>(player).hand;


        std::random_device rd;
        std::mt19937 range(rd());
        std::uniform_int_distribution<std::mt19937::result_type> distribution(0, deck.size() - 1);

        for (int i = 0; i < 2; i++) {
            const int random_card = distribution(range);
            int j = 0;
            for (const auto card: deck) {
                if (j == random_card) {
                    hand.push_back(registry.get<Card>(card));
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

void increase_by_pass_by_reference(int &value) {
    value++;
}

auto increase_by_pass_by_value(int value) {
    return value + 1;
}

int main() {

    std::cout << "Hello, World!" << std::endl;

    entt::registry registry;

    initialize_deck(registry);


    initialize_players(registry);


    const auto deck = registry.view<Card>();
    const auto players = registry.view<Player>();

    while (deck.size() > 2 * players.size()) {

        for(auto player: players) {
            auto player_hand = registry.get<Player>(player).hand;
            if(player_hand.size() > 0) {
                player_hand.clear();
                player_hand.clear();
            }
        }

        deal_cards(registry);

        const auto players = registry.view<Player>();
        for (const auto player: players) {
            const auto &hand = registry.get<Player>(player).hand;

            std::cout << registry.get<Player>(player).name << " has " << hand[0].name << " and " << hand[1].name
                    << std::endl;
        }

        std::cout << "Choose a card to play:" << std::endl;
        std::string chosen_card_input;
        std::cin >> chosen_card_input;

        auto chosen_card_value = 0;
        auto oppenent_card_value = 0;
        for (const auto player: players) {
            if (const auto opponent = registry.get<Player>(player).name == "Opponent") {
                auto &hand = registry.get<Player>(player).hand;
                std::random_device rd;
                std::mt19937 range(rd());
                std::uniform_int_distribution<std::mt19937::result_type> distribution(0, std::size(hand) - 1);

                const int random_card = distribution(range);
                std::cout << "Opponent played " << hand[random_card].name << std::endl;
                oppenent_card_value = hand[random_card].value;
            } else {
                auto &hand = registry.get<Player>(player).hand;
                for (auto card: hand) {
                    if (card.name == chosen_card_input) {
                        std::cout << "You played " << chosen_card_input << std::endl;
                        chosen_card_value = card.value;
                    }
                }

                if (chosen_card_value > oppenent_card_value) {
                    std::cout << "You won!" << std::endl;
                } else if (chosen_card_value < oppenent_card_value) {
                    std::cout << "You lost!" << std::endl;
                } else {
                    std::cout << "It's a tie!" << std::endl;
                }
            }
        }
    }
    std::cout << "Not enough cards in the deck!" << std::endl;


    return 0;
}
