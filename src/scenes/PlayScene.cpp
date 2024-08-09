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

#define CARD_WIDTH 42
#define CARD_HEIGHT 60

PlayScene::PlayScene() :
        _image(entt::null),
        Scene() {}

PlayScene::~PlayScene() = default;

void PlayScene::setup() {
    Scene::setup();
    setBackgroundColour(Color{55, 138, 249, 255});
//    toggleDebug();
//    _image = _ecs.registry.create();
//    _ecs.registry.emplace<TransformComponent>(_image, 100, 300, 588, 240);
//    _ecs.registry.emplace<SpriteComponent>(_image, "cardsLarge_tilemap", 0, 0, 588, 240);

    auto cardBack = new SpriteComponent("cardsLarge_tilemap", 546, 0, _cardWidth, _cardHeight);

    auto cardAceOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardAceOfHeart, "Ace of Heart", 1);
    _ecs.registry.emplace<TransformComponent>(cardAceOfHeart, 0, 0, 42, 60);
    auto cardAceOfHeartSprite = new SpriteComponent("cardsLarge_tilemap", 0, 0, _cardWidth, _cardHeight);
    auto cardAceOfHeartSprites = new std::unordered_map<std::string, SpriteComponent>();
    cardAceOfHeartSprites->insert(std::make_pair("front", *cardAceOfHeartSprite));
    cardAceOfHeartSprites->insert(std::make_pair("back", *cardBack));
    _ecs.registry.emplace<MultipleSpriteComponent>(cardAceOfHeart, *cardAceOfHeartSprites, "back");

    auto cardTwoOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTwoOfHeart, "Two of Heart", 2);
    _ecs.registry.emplace<TransformComponent>(cardTwoOfHeart, 42, 0, 42, 60);
    auto cardTwoOfHeartSprite = new SpriteComponent("cardsLarge_tilemap", 42, 0, _cardWidth, _cardHeight);
    auto cardTwoOfHeartSprites = new std::unordered_map<std::string, SpriteComponent>();
    cardTwoOfHeartSprites->insert(std::make_pair("front", *cardTwoOfHeartSprite));
    cardTwoOfHeartSprites->insert(std::make_pair("back", *cardBack));
    _ecs.registry.emplace<MultipleSpriteComponent>(cardTwoOfHeart, *cardTwoOfHeartSprites, "back");

    auto cardThreeOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardThreeOfHeart, "Three of Heart", 3);
    _ecs.registry.emplace<TransformComponent>(cardThreeOfHeart, 84, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardThreeOfHeart, "cardsLarge_tilemap", 84, 0, 42, 60);

    auto cardFourOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFourOfHeart, "Four of Heart", 4);
    _ecs.registry.emplace<TransformComponent>(cardFourOfHeart, 126, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFourOfHeart, "cardsLarge_tilemap", 126, 0, 42, 60);

    auto cardFiveOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFiveOfHeart, "Five of Heart", 5);
    _ecs.registry.emplace<TransformComponent>(cardFiveOfHeart, 168, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFiveOfHeart, "cardsLarge_tilemap", 168, 0, 42, 60);

    auto cardSixOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSixOfHeart, "Six of Heart", 6);
    _ecs.registry.emplace<TransformComponent>(cardSixOfHeart, 210, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSixOfHeart, "cardsLarge_tilemap", 210, 0, 42, 60);

    auto cardSevenOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSevenOfHeart, "Seven of Heart", 7);
    _ecs.registry.emplace<TransformComponent>(cardSevenOfHeart, 252, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSevenOfHeart, "cardsLarge_tilemap", 252, 0, 42, 60);

    auto cardEightOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardEightOfHeart, "Eight of Heart", 8);
    _ecs.registry.emplace<TransformComponent>(cardEightOfHeart, 294, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardEightOfHeart, "cardsLarge_tilemap", 294, 0, 42, 60);

    auto cardNineOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardNineOfHeart, "Nine of Heart", 9);
    _ecs.registry.emplace<TransformComponent>(cardNineOfHeart, 336, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardNineOfHeart, "cardsLarge_tilemap", 336, 0, 42, 60);

    auto cardTenOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTenOfHeart, "Ten of Heart", 10);
    _ecs.registry.emplace<TransformComponent>(cardTenOfHeart, 378, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTenOfHeart, "cardsLarge_tilemap", 378, 0, 42, 60);

    auto cardJackOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardJackOfHeart, "Jack of Heart", 11);
    _ecs.registry.emplace<TransformComponent>(cardJackOfHeart, 420, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardJackOfHeart, "cardsLarge_tilemap", 420, 0, 42, 60);

    auto cardQueenOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardQueenOfHeart, "Queen of Heart", 12);
    _ecs.registry.emplace<TransformComponent>(cardQueenOfHeart, 462, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardQueenOfHeart, "cardsLarge_tilemap", 462, 0, 42, 60);

    auto cardKingOfHeart = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardKingOfHeart, "King of Heart", 13);
    _ecs.registry.emplace<TransformComponent>(cardKingOfHeart, 504, 0, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardKingOfHeart, "cardsLarge_tilemap", 504, 0, 42, 60);

    auto cardAceOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardAceOfDiamond, "Ace of Diamond", 1);
    _ecs.registry.emplace<TransformComponent>(cardAceOfDiamond, 0, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardAceOfDiamond, "cardsLarge_tilemap", 0, 60, 42, 60);

    auto cardTwoOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTwoOfDiamond, "Two of Diamond", 2);
    _ecs.registry.emplace<TransformComponent>(cardTwoOfDiamond, 42, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTwoOfDiamond, "cardsLarge_tilemap", 42, 60, 42, 60);

    auto cardThreeOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardThreeOfDiamond, "Three of Diamond", 3);
    _ecs.registry.emplace<TransformComponent>(cardThreeOfDiamond, 84, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardThreeOfDiamond, "cardsLarge_tilemap", 84, 60, 42, 60);

    auto cardFourOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFourOfDiamond, "Four of Diamond", 4);
    _ecs.registry.emplace<TransformComponent>(cardFourOfDiamond, 126, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFourOfDiamond, "cardsLarge_tilemap", 126, 60, 42, 60);

    auto cardFiveOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFiveOfDiamond, "Five of Diamond", 5);
    _ecs.registry.emplace<TransformComponent>(cardFiveOfDiamond, 168, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFiveOfDiamond, "cardsLarge_tilemap", 168, 60, 42, 60);

    auto cardSixOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSixOfDiamond, "Six of Diamond", 6);
    _ecs.registry.emplace<TransformComponent>(cardSixOfDiamond, 210, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSixOfDiamond, "cardsLarge_tilemap", 210, 60, 42, 60);

    auto cardSevenOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSevenOfDiamond, "Seven of Diamond", 7);
    _ecs.registry.emplace<TransformComponent>(cardSevenOfDiamond, 252, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSevenOfDiamond, "cardsLarge_tilemap", 252, 60, 42, 60);

    auto cardEightOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardEightOfDiamond, "Eight of Diamond", 8);
    _ecs.registry.emplace<TransformComponent>(cardEightOfDiamond, 294, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardEightOfDiamond, "cardsLarge_tilemap", 294, 60, 42, 60);

    auto cardNineOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardNineOfDiamond, "Nine of Diamond", 9);
    _ecs.registry.emplace<TransformComponent>(cardNineOfDiamond, 336, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardNineOfDiamond, "cardsLarge_tilemap", 336, 60, 42, 60);

    auto cardTenOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTenOfDiamond, "Ten of Diamond", 10);
    _ecs.registry.emplace<TransformComponent>(cardTenOfDiamond, 378, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTenOfDiamond, "cardsLarge_tilemap", 378, 60, 42, 60);

    auto cardJackOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardJackOfDiamond, "Jack of Diamond", 11);
    _ecs.registry.emplace<TransformComponent>(cardJackOfDiamond, 420, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardJackOfDiamond, "cardsLarge_tilemap", 420, 60, 42, 60);


    auto cardQueenOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardQueenOfDiamond, "Queen of Diamond", 12);
    _ecs.registry.emplace<TransformComponent>(cardQueenOfDiamond, 462, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardQueenOfDiamond, "cardsLarge_tilemap", 462, 60, 42, 60);

    auto cardKingOfDiamond = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardKingOfDiamond, "King of Diamond", 13);
    _ecs.registry.emplace<TransformComponent>(cardKingOfDiamond, 504, 60, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardKingOfDiamond, "cardsLarge_tilemap", 504, 60, 42, 60);


    auto cardAceOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardAceOfClub, "Ace of Club", 1);
    _ecs.registry.emplace<TransformComponent>(cardAceOfClub, 0, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardAceOfClub, "cardsLarge_tilemap", 0, 120, 42, 60);

    auto cardTwoOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTwoOfClub, "Two of Club", 2);
    _ecs.registry.emplace<TransformComponent>(cardTwoOfClub, 42, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTwoOfClub, "cardsLarge_tilemap", 42, 120, 42, 60);

    auto cardThreeOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardThreeOfClub, "Three of Club", 3);
    _ecs.registry.emplace<TransformComponent>(cardThreeOfClub, 84, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardThreeOfClub, "cardsLarge_tilemap", 84, 120, 42, 60);

    auto cardFourOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFourOfClub, "Four of Club", 4);
    _ecs.registry.emplace<TransformComponent>(cardFourOfClub, 126, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFourOfClub, "cardsLarge_tilemap", 126, 120, 42, 60);

    auto cardFiveOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFiveOfClub, "Five of Club", 5);
    _ecs.registry.emplace<TransformComponent>(cardFiveOfClub, 168, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFiveOfClub, "cardsLarge_tilemap", 168, 120, 42, 60);

    auto cardSixOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSixOfClub, "Six of Club", 6);
    _ecs.registry.emplace<TransformComponent>(cardSixOfClub, 210, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSixOfClub, "cardsLarge_tilemap", 210, 120, 42, 60);

    auto cardSevenOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSevenOfClub, "Seven of Club", 7);
    _ecs.registry.emplace<TransformComponent>(cardSevenOfClub, 252, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSevenOfClub, "cardsLarge_tilemap", 252, 120, 42, 60);

    auto cardEightOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardEightOfClub, "Eight of Club", 8);
    _ecs.registry.emplace<TransformComponent>(cardEightOfClub, 294, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardEightOfClub, "cardsLarge_tilemap", 294, 120, 42, 60);

    auto cardNineOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardNineOfClub, "Nine of Club", 9);
    _ecs.registry.emplace<TransformComponent>(cardNineOfClub, 336, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardNineOfClub, "cardsLarge_tilemap", 336, 120, 42, 60);

    auto cardTenOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTenOfClub, "Ten of Club", 10);
    _ecs.registry.emplace<TransformComponent>(cardTenOfClub, 378, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTenOfClub, "cardsLarge_tilemap", 378, 120, 42, 60);

    auto cardJackOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardJackOfClub, "Jack of Club", 11);
    _ecs.registry.emplace<TransformComponent>(cardJackOfClub, 420, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardJackOfClub, "cardsLarge_tilemap", 420, 120, 42, 60);

    auto cardQueenOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardQueenOfClub, "Queen of Club", 12);
    _ecs.registry.emplace<TransformComponent>(cardQueenOfClub, 462, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardQueenOfClub, "cardsLarge_tilemap", 462, 120, 42, 60);

    auto cardKingOfClub = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardKingOfClub, "King of Club", 13);
    _ecs.registry.emplace<TransformComponent>(cardKingOfClub, 504, 120, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardKingOfClub, "cardsLarge_tilemap", 504, 120, 42, 60);


    auto cardAceOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardAceOfSpade, "Ace of Spade", 1);
    _ecs.registry.emplace<TransformComponent>(cardAceOfSpade, 0, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardAceOfSpade, "cardsLarge_tilemap", 0, 180, 42, 60);

    auto cardTwoOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTwoOfSpade, "Two of Spade", 2);
    _ecs.registry.emplace<TransformComponent>(cardTwoOfSpade, 42, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTwoOfSpade, "cardsLarge_tilemap", 42, 180, 42, 60);


    auto cardThreeOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardThreeOfSpade, "Three of Spade", 3);
    _ecs.registry.emplace<TransformComponent>(cardThreeOfSpade, 84, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardThreeOfSpade, "cardsLarge_tilemap", 84, 180, 42, 60);


    auto cardFourOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFourOfSpade, "Four of Spade", 4);
    _ecs.registry.emplace<TransformComponent>(cardFourOfSpade, 126, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFourOfSpade, "cardsLarge_tilemap", 126, 180, 42, 60);


    auto cardFiveOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardFiveOfSpade, "Five of Spade", 5);
    _ecs.registry.emplace<TransformComponent>(cardFiveOfSpade, 168, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardFiveOfSpade, "cardsLarge_tilemap", 168, 180, 42, 60);


    auto cardSixOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSixOfSpade, "Six of Spade", 6);
    _ecs.registry.emplace<TransformComponent>(cardSixOfSpade, 210, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSixOfSpade, "cardsLarge_tilemap", 210, 180, 42, 60);


    auto cardSevenOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardSevenOfSpade, "Seven of Spade", 7);
    _ecs.registry.emplace<TransformComponent>(cardSevenOfSpade, 252, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardSevenOfSpade, "cardsLarge_tilemap", 252, 180, 42, 60);


    auto cardEightOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardEightOfSpade, "Eight of Spade", 8);
    _ecs.registry.emplace<TransformComponent>(cardEightOfSpade, 294, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardEightOfSpade, "cardsLarge_tilemap", 294, 180, 42, 60);


    auto cardNineOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardNineOfSpade, "Nine of Spade", 9);
    _ecs.registry.emplace<TransformComponent>(cardNineOfSpade, 336, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardNineOfSpade, "cardsLarge_tilemap", 336, 180, 42, 60);


    auto cardTenOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardTenOfSpade, "Ten of Spade", 10);
    _ecs.registry.emplace<TransformComponent>(cardTenOfSpade, 378, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardTenOfSpade, "cardsLarge_tilemap", 378, 180, 42, 60);


    auto cardJackOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardJackOfSpade, "Jack of Spade", 11);
    _ecs.registry.emplace<TransformComponent>(cardJackOfSpade, 420, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardJackOfSpade, "cardsLarge_tilemap", 420, 180, 42, 60);


    auto cardQueenOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardQueenOfSpade, "Queen of Spade", 12);
    _ecs.registry.emplace<TransformComponent>(cardQueenOfSpade, 462, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardQueenOfSpade, "cardsLarge_tilemap", 462, 180, 42, 60);


    auto cardKingOfSpade = _ecs.registry.create();
    _ecs.registry.emplace<CardComponent>(cardKingOfSpade, "King of Spade", 13);
    _ecs.registry.emplace<TransformComponent>(cardKingOfSpade, 504, 180, 42, 60);
    _ecs.registry.emplace<SpriteComponent>(cardKingOfSpade, "cardsLarge_tilemap", 504, 180, 42, 60);

}

void PlayScene::update(float deltaTime, Input &input) {
    Scene::update(deltaTime, input);
}




