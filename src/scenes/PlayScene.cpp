/**
 * @file PlayScene.cpp
 * @brief Implementation file for the PlayScene class.
 *
 * This file contains the implementation of the PlayScene class which is responsible for managing the play scene in the game.
 * The PlayScene class is responsible for setting up the play scene, updating the play scene, and handling input for the play scene.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-21
 */

#include "PlayScene.h"

PlayScene::PlayScene() : Scene() {}

PlayScene::~PlayScene() = default;

void PlayScene::setup() {
    Scene::setup();
    setBackgroundColour(Color{55, 138, 249, 255});
//    toggleDebug();

    // calculate the initial position of the cards
    _initialPosition.x = static_cast<float>(GLOBAL_WINDOW_WIDTH - _cardWidth * 13) / 2;
    _initialPosition.y = static_cast<float>(GLOBAL_WINDOW_HEIGHT - _cardHeight * 4) / 2;

    LOG_INFO("initial position: {0}, {1}", _initialPosition.x, _initialPosition.y);

    // Define card data
    std::vector<CardInfo> cards = {
            {"Ace of Heart",     1,  0,   0},
            {"Two of Heart",     2,  42,  0},
            {"Three of Heart",   3,  84,  0},
            {"Four of Heart",    4,  126, 0},
            {"Five of Heart",    5,  168, 0},
            {"Six of Heart",     6,  210, 0},
            {"Seven of Heart",   7,  252, 0},
            {"Eight of Heart",   8,  294, 0},
            {"Nine of Heart",    9,  336, 0},
            {"Ten of Heart",     10, 378, 0},
            {"Jack of Heart",    11, 420, 0},
            {"Queen of Heart",   12, 462, 0},
            {"King of Heart",    13, 504, 0},
            {"Ace of Diamond",   1,  0,   60},
            {"Two of Diamond",   2,  42,  60},
            {"Three of Diamond", 3,  84,  60},
            {"Four of Diamond",  4,  126, 60},
            {"Five of Diamond",  5,  168, 60},
            {"Six of Diamond",   6,  210, 60},
            {"Seven of Diamond", 7,  252, 60},
            {"Eight of Diamond", 8,  294, 60},
            {"Nine of Diamond",  9,  336, 60},
            {"Ten of Diamond",   10, 378, 60},
            {"Jack of Diamond",  11, 420, 60},
            {"Queen of Diamond", 12, 462, 60},
            {"King of Diamond",  13, 504, 60},
            {"Ace of Club",      1,  0,   120},
            {"Two of Club",      2,  42,  120},
            {"Three of Club",    3,  84,  120},
            {"Four of Club",     4,  126, 120},
            {"Five of Club",     5,  168, 120},
            {"Six of Club",      6,  210, 120},
            {"Seven of Club",    7,  252, 120},
            {"Eight of Club",    8,  294, 120},
            {"Nine of Club",     9,  336, 120},
            {"Ten of Club",      10, 378, 120},
            {"Jack of Club",     11, 420, 120},
            {"Queen of Club",    12, 462, 120},
            {"King of Club",     13, 504, 120},
            {"Ace of Spade",     1,  0,   180},
            {"Two of Spade",     2,  42,  180},
            {"Three of Spade",   3,  84,  180},
            {"Four of Spade",    4,  126, 180},
            {"Five of Spade",    5,  168, 180},
            {"Six of Spade",     6,  210, 180},
            {"Seven of Spade",   7,  252, 180},
            {"Eight of Spade",   8,  294, 180},
            {"Nine of Spade",    9,  336, 180},
            {"Ten of Spade",     10, 378, 180},
            {"Jack of Spade",    11, 420, 180},
            {"Queen of Spade",   12, 462, 180},
            {"King of Spade",    13, 504, 180}
    };

    // Shuffle the cards
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);

    // Create cards using the helper function
    for (int i = 0; i < cards.size(); ++i) {
        float positionX = _initialPosition.x + static_cast<float>(_cardWidth * (i % 13));
        float positionY = _initialPosition.y + static_cast<float>(_cardHeight * (i / 13));
        createCard(cards[i], positionX, positionY);
    }

    // create a player
    _player = _ecs.registry.create();
    _ecs.registry.emplace<PlayerComponent>(_player, "Player 1");
    _ecs.registry.emplace<TransformComponent>(_player, 100, 100, 42, 60);

    _playerHand = _ecs.registry.create();
    _ecs.registry.emplace<TransformComponent>(_playerHand, 325, 513, _cardWidth + 10, _cardHeight + 10);
    _ecs.registry.emplace<SpriteComponent>(_playerHand, "button_square_flat", 0, 0, 128, 128);

    _playerHead = _ecs.registry.create();
    _ecs.registry.emplace<TransformComponent>(_playerHead, 408, 513, _cardWidth + 10, _cardHeight + 10);
    _ecs.registry.emplace<SpriteComponent>(_playerHead, "button_square_flat", 0, 0, 128, 128);

    _npcHand = _ecs.registry.create();
    _ecs.registry.emplace<TransformComponent>(_npcHand, 325, 63, _cardWidth + 10, _cardHeight + 10);
    _ecs.registry.emplace<SpriteComponent>(_npcHand, "button_square_flat", 0, 0, 128, 128);

    _npcHead = _ecs.registry.create();
    _ecs.registry.emplace<TransformComponent>(_npcHead, 408, 63, _cardWidth + _margin, _cardHeight + _margin);
    _ecs.registry.emplace<SpriteComponent>(_npcHead, "button_square_flat", 0, 0, 128, 128);
}

void PlayScene::update(float deltaTime, Input &input) {
    Scene::update(deltaTime, input);
    int mouseX, mouseY;
    input.getMousePosition(mouseX, mouseY);
    if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
        LOG_INFO("mouse left button pressed at: {0}, {1}", mouseX, mouseY);
    }
    // snapping card to hand and head position
    auto draggableView = _ecs.registry.view<TransformComponent, MultipleSpriteComponent>();
    auto &targetHandTransform = _ecs.registry.get<TransformComponent>(_playerHand);
    auto &targetHeadTransform = _ecs.registry.get<TransformComponent>(_playerHead);
    auto &targetNpcHandTransform = _ecs.registry.get<TransformComponent>(_npcHand);
    auto &targetNpcHeadTransform = _ecs.registry.get<TransformComponent>(_npcHead);

    for (auto entity: draggableView) {
        auto &transform = draggableView.get<TransformComponent>(entity);
        auto &multipleSpriteComponent = draggableView.get<MultipleSpriteComponent>(entity);

        if (std::abs(transform.position.x - targetHandTransform.position.x) < static_cast<float>(_cardWidth) &&
            std::abs(transform.position.y - targetHandTransform.position.y) < static_cast<float>(_cardHeight)) {
            transform.position.x = targetHandTransform.position.x + 5;
            transform.position.y = targetHandTransform.position.y + 5;
            if (transform.position.x == targetHandTransform.position.x + 5 &&
                transform.position.y == targetHandTransform.position.y + 5) {
                multipleSpriteComponent.currentSprite = "front";
                multipleSpriteComponent.layer += 1;
                LOG_INFO("layer: {0}", multipleSpriteComponent.layer);
            }
        }
        if (std::abs(transform.position.x - targetHeadTransform.position.x) < static_cast<float>(_cardWidth) &&
            std::abs(transform.position.y - targetHeadTransform.position.y) < static_cast<float>(_cardHeight)) {
            transform.position.x = targetHeadTransform.position.x + 5;
            transform.position.y = targetHeadTransform.position.y + 5;
            if (transform.position.x == targetHeadTransform.position.x + 5 &&
                transform.position.y == targetHeadTransform.position.y + 5) {
                multipleSpriteComponent.currentSprite = "back";
                multipleSpriteComponent.layer += 1;
                LOG_INFO("layer: {0}", multipleSpriteComponent.layer);
            }
        }
        if (std::abs(transform.position.x - targetNpcHandTransform.position.x) < static_cast<float>(_cardWidth) &&
            std::abs(transform.position.y - targetNpcHandTransform.position.y) < static_cast<float>(_cardHeight)) {
            transform.position.x = targetNpcHandTransform.position.x + 5;
            transform.position.y = targetNpcHandTransform.position.y + 5;
            if (transform.position.x == targetHandTransform.position.x + 5 &&
                transform.position.y == targetHandTransform.position.y + 5) {
                multipleSpriteComponent.currentSprite = "front";
                multipleSpriteComponent.layer += 1;
                LOG_INFO("layer: {0}", multipleSpriteComponent.layer);
            }
        }
        if (std::abs(transform.position.x - targetNpcHeadTransform.position.x) < static_cast<float>(_cardWidth) &&
            std::abs(transform.position.y - targetNpcHeadTransform.position.y) < static_cast<float>(_cardHeight)) {
            transform.position.x = targetNpcHeadTransform.position.x + 5;
            transform.position.y = targetNpcHeadTransform.position.y + 5;
            if (transform.position.x == targetHeadTransform.position.x + 5 &&
                transform.position.y == targetHeadTransform.position.y + 5) {
                multipleSpriteComponent.currentSprite = "back";
                multipleSpriteComponent.layer += 1;
                LOG_INFO("layer: {0}", multipleSpriteComponent.layer);
            }
        }
    }

//    auto view = _ecs.registry.view<MultipleSpriteComponent, ClickableComponent>();
//
//    for (auto entity: view) {
//        auto &multipleSpriteComponent = view.get<MultipleSpriteComponent>(entity);
//        auto &clickableComponent = view.get<ClickableComponent>(entity);
//        if (clickableComponent.isClicked) {
//            if (multipleSpriteComponent.currentSprite == "front") {
//                multipleSpriteComponent.currentSprite = "back";
//            } else {
//                multipleSpriteComponent.currentSprite = "front";
//            }
//            clickableComponent.isClicked = false;
//        }
//    }
}


// Helper function to create a card entity
entt::entity PlayScene::createCard(const CardInfo &cardInfo, float positionX, float positionY) {
    auto card = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(card, cardInfo.name, cardInfo.value);
    _ecs.registry.emplace<TransformComponent>(card, positionX, positionY, _cardWidth, _cardHeight);

    SpriteComponent cardFrontSprite("cardsLarge_tilemap", cardInfo.spriteX, cardInfo.spriteY, _cardWidth,
                                    _cardHeight);
    SpriteComponent cardBackSprite("cardsLarge_tilemap", 546, 0, _cardWidth, _cardHeight);

    std::unordered_map<std::string, SpriteComponent> cardSprites;
    cardSprites.emplace("front", cardFrontSprite);
    cardSprites.emplace("back", cardBackSprite);

    _ecs.registry.emplace<MultipleSpriteComponent>(card, cardSprites, "back", 1);
    _ecs.registry.emplace<DraggableComponent>(card);
    _ecs.registry.emplace<ClickableComponent>(card);

    return card;
}

