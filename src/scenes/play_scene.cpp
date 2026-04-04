/**
 * @file    play_scene.cpp
 * @brief   Source file for the SharkCardGame play scene.
 * @details This file defines the SharkCardGame table scene using the current
 *          Cbit2D ECS, drag system, and UI system.
 * @author  Nur Akmal bin Jalil
 * @date    2026-04-05
 */

#include "play_scene.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <utility>

#include <entt/entity/registry.hpp>

#include "cbit/core/input.hpp"
#include "cbit/core/logger.hpp"
#include "cbit/ecs/components.hpp"
#include "cbit/ecs/entity_component_system.hpp"

namespace shark_card_game::scenes {

namespace {

constexpr float kCardWidth = 42.0F;
constexpr float kCardHeight = 60.0F;
constexpr float kCardSpacing = 6.0F;
constexpr float kSlotSizePadding = 12.0F;
constexpr float kSnapDistance = 58.0F;
constexpr glm::vec2 kCardBackSourcePosition {546.0F, 0.0F};
constexpr const char* kCardTexturePath = "resources/images/cardsLarge_tilemap.png";
constexpr const char* kSquareSlotTexturePath = "resources/images/button_square_flat.png";

/**
 * @brief Returns whether a slot should display cards face up.
 * @param slotKind Logical slot role.
 * @return `true` when snapped cards should show their front face.
 */
bool shouldShowFrontFace(const SlotKind slotKind)
{
    return slotKind == SlotKind::PlayerHand || slotKind == SlotKind::NonPlayableCharacterHand;
}

} // namespace

/**
 * @brief Creates the play scene with a menu callback.
 * @param onReturnToMenu Callback invoked when the menu button is pressed.
 */
PlayScene::PlayScene(std::function<void()> onReturnToMenu)
    : _onReturnToMenu(std::move(onReturnToMenu)),
      _matchState(gameplay::createInitialMatchState(2, 0))
{
}

/**
 * @brief Initializes the play scene entities.
 */
void PlayScene::initialize()
{
    auto title = world.addGameObject("PlayTitle");
    auto& titleTransform = title.getComponent<cbit::ecs::TransformComponent>();
    titleTransform.position = {640.0F, 42.0F};

    auto& titleText = title.addComponent<cbit::ecs::TextComponent>();
    titleText.content = "Shark Card Game";
    titleText.fontPath = "resources/fonts/Kenney_Future.ttf";
    titleText.fontSize = 28.0F;
    titleText.color = {244, 246, 255, 255};

    auto instructions = world.addGameObject("Instructions");
    auto& instructionsTransform = instructions.getComponent<cbit::ecs::TransformComponent>();
    instructionsTransform.position = {640.0F, 80.0F};

    auto& instructionsText = instructions.addComponent<cbit::ecs::TextComponent>();
    instructionsText.content = "Drag cards into the highlighted slots. Hand slots reveal cards, head slots keep them hidden.";
    instructionsText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    instructionsText.fontSize = 18.0F;
    instructionsText.color = {210, 226, 240, 255};

    auto menuButton = world.addGameObject("BackToMenuButton");
    auto& menuButtonTransform = menuButton.getComponent<cbit::ecs::TransformComponent>();
    menuButtonTransform.position = {1130.0F, 52.0F};

    auto& menuButtonComponent = menuButton.addComponent<cbit::ecs::ButtonComponent>();
    menuButtonComponent.size = {180.0F, 52.0F};
    menuButtonComponent.backgroundColor = {34, 45, 67, 255};
    menuButtonComponent.hoverColor = {48, 64, 93, 255};
    menuButtonComponent.pressedColor = {22, 31, 46, 255};
    menuButtonComponent.borderColor = {231, 207, 115, 255};
    menuButtonComponent.onClick = _onReturnToMenu;

    auto& menuButtonText = menuButton.addComponent<cbit::ecs::TextComponent>();
    menuButtonText.content = "Menu";
    menuButtonText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    menuButtonText.fontSize = 20.0F;
    menuButtonText.color = {255, 255, 255, 255};

    createMatchHud();
    createBoardSlots();
    createDeck();
    refreshMatchHud();

    world.addSystem([](cbit::ecs::EntityComponentSystem& ecs) {
        if (!cbit2d::core::Input::isMouseButtonReleased(cbit2d::core::MouseButton::Left)) {
            return;
        }

        auto& registry = ecs.getRegistry();
        auto slotView = registry.view<const cbit::ecs::TransformComponent, const SlotComponent>();
        auto cardView = registry.view<cbit::ecs::TransformComponent, cbit::ecs::SpriteComponent, CardComponent, cbit::ecs::DragableComponent>();

        for (const auto cardEntity : cardView) {
            auto [cardTransform, cardSprite, card, dragable] = cardView.get<
                cbit::ecs::TransformComponent,
                cbit::ecs::SpriteComponent,
                CardComponent,
                cbit::ecs::DragableComponent
            >(cardEntity);

            if (dragable.isDragging) {
                continue;
            }

            entt::entity closestSlot = entt::null;
            float closestDistanceSquared = kSnapDistance * kSnapDistance;
            for (const auto slotEntity : slotView) {
                const auto& slotTransform = slotView.get<const cbit::ecs::TransformComponent>(slotEntity);
                const float dx = cardTransform.position.x - slotTransform.position.x;
                const float dy = cardTransform.position.y - slotTransform.position.y;
                const float distanceSquared = (dx * dx) + (dy * dy);
                if (distanceSquared > closestDistanceSquared) {
                    continue;
                }

                closestDistanceSquared = distanceSquared;
                closestSlot = slotEntity;
            }

            if (closestSlot == entt::null) {
                continue;
            }

            const auto& slotTransform = slotView.get<const cbit::ecs::TransformComponent>(closestSlot);
            const auto& slot = slotView.get<const SlotComponent>(closestSlot);
            const auto& slotId = registry.get<cbit::ecs::IdComponent>(closestSlot);

            cardTransform.position = slotTransform.position;
            card.faceUp = shouldShowFrontFace(slot.kind);
            card.snappedSlotId = slotId.id;
            cardSprite.sourcePosition = card.faceUp ? card.frontSourcePosition : kCardBackSourcePosition;
        }
    });

    world.addSystem([this](cbit::ecs::EntityComponentSystem& ecs) {
        auto& registry = ecs.getRegistry();
        auto cardView = registry.view<cbit::ecs::TransformComponent, cbit::ecs::SpriteComponent, CardComponent, cbit::ecs::DragableComponent>();
        for (const auto entity : cardView) {
            auto [transform, sprite, card, dragable] = cardView.get<
                cbit::ecs::TransformComponent,
                cbit::ecs::SpriteComponent,
                CardComponent,
                cbit::ecs::DragableComponent
            >(entity);

            if (!dragable.isDragging) {
                continue;
            }

            card.snappedSlotId = 0;
            card.faceUp = true;
            sprite.sourcePosition = card.frontSourcePosition;
            sprite.renderOrder = 10;
            transform.rotation = std::sin(_deltaTimeSeconds * 8.0F) * 2.0F;
        }

        int renderOrder = 1;
        for (const auto entity : cardView) {
            auto& dragable = cardView.get<cbit::ecs::DragableComponent>(entity);
            if (dragable.isDragging) {
                continue;
            }

            auto& sprite = cardView.get<cbit::ecs::SpriteComponent>(entity);
            auto& transform = cardView.get<cbit::ecs::TransformComponent>(entity);
            sprite.renderOrder = renderOrder++;
            transform.rotation = 0.0F;
        }
    });

    cbit2d::core::Logger::info("SharkCardGame play scene initialized");
}

/**
 * @brief Updates the play scene.
 * @param deltaTimeSeconds Elapsed time since the previous frame.
 */
void PlayScene::update(float deltaTimeSeconds)
{
    _deltaTimeSeconds = deltaTimeSeconds;
    refreshMatchHud();
    world.update(deltaTimeSeconds);
}

/**
 * @brief Creates all static slot entities for the card table.
 */
void PlayScene::createBoardSlots()
{
    createSlot("NpcHandSlot", {450.0F, 180.0F}, SlotKind::NonPlayableCharacterHand);
    createSlot("NpcHeadSlot", {550.0F, 180.0F}, SlotKind::NonPlayableCharacterHead);
    createSlot("PlayerHandSlot", {450.0F, 560.0F}, SlotKind::PlayerHand);
    createSlot("PlayerHeadSlot", {550.0F, 560.0F}, SlotKind::PlayerHead);

    auto npcLabel = world.addGameObject("NpcLabel");
    auto& npcLabelTransform = npcLabel.getComponent<cbit::ecs::TransformComponent>();
    npcLabelTransform.position = {500.0F, 125.0F};

    auto& npcLabelText = npcLabel.addComponent<cbit::ecs::TextComponent>();
    npcLabelText.content = _matchState.players[1].displayName;
    npcLabelText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    npcLabelText.fontSize = 20.0F;
    npcLabelText.color = {255, 255, 255, 255};

    auto playerLabel = world.addGameObject("PlayerLabel");
    auto& playerLabelTransform = playerLabel.getComponent<cbit::ecs::TransformComponent>();
    playerLabelTransform.position = {500.0F, 615.0F};

    auto& playerLabelText = playerLabel.addComponent<cbit::ecs::TextComponent>();
    playerLabelText.content = _matchState.players[0].displayName;
    playerLabelText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    playerLabelText.fontSize = 20.0F;
    playerLabelText.color = {255, 255, 255, 255};
}

/**
 * @brief Creates the deck of draggable cards.
 */
void PlayScene::createDeck()
{
    auto deckTitle = world.addGameObject("DeckTitle");
    auto& deckTitleTransform = deckTitle.getComponent<cbit::ecs::TransformComponent>();
    deckTitleTransform.position = {640.0F, 142.0F};

    auto& deckTitleText = deckTitle.addComponent<cbit::ecs::TextComponent>();
    deckTitleText.content = "Deck";
    deckTitleText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    deckTitleText.fontSize = 24.0F;
    deckTitleText.color = {255, 255, 255, 255};

    _deckOrigin = {
        (1280.0F - ((kCardWidth + kCardSpacing) * 13.0F)) * 0.5F + (kCardWidth * 0.5F),
        248.0F
    };

    for (std::size_t index = 0; index < _matchState.round.shuffledDeck.size(); ++index) {
        const float column = static_cast<float>(index % 13);
        const float row = static_cast<float>(index / 13);
        const glm::vec2 position {
            _deckOrigin.x + (column * (kCardWidth + kCardSpacing)),
            _deckOrigin.y + (row * (kCardHeight + kCardSpacing))
        };
        createCard(_matchState.round.shuffledDeck[index], position);
    }
}

/**
 * @brief Creates HUD text that reflects the current match state.
 */
void PlayScene::createMatchHud()
{
    auto phaseText = world.addGameObject("PhaseInfoText");
    phaseText.getComponent<cbit::ecs::TransformComponent>().position = {150.0F, 44.0F};
    auto& phaseTextComponent = phaseText.addComponent<cbit::ecs::TextComponent>();
    phaseTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    phaseTextComponent.fontSize = 20.0F;
    phaseTextComponent.color = {249, 214, 119, 255};
    phaseTextComponent.centered = false;
    _phaseTextId = phaseText.getComponent<cbit::ecs::IdComponent>().id;

    auto roundText = world.addGameObject("RoundInfoText");
    roundText.getComponent<cbit::ecs::TransformComponent>().position = {150.0F, 70.0F};
    auto& roundTextComponent = roundText.addComponent<cbit::ecs::TextComponent>();
    roundTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    roundTextComponent.fontSize = 18.0F;
    roundTextComponent.color = {214, 228, 240, 255};
    roundTextComponent.centered = false;
    _roundTextId = roundText.getComponent<cbit::ecs::IdComponent>().id;

    auto localPlayerStatus = world.addGameObject("LocalPlayerStatusText");
    localPlayerStatus.getComponent<cbit::ecs::TransformComponent>().position = {870.0F, 520.0F};
    auto& localPlayerStatusText = localPlayerStatus.addComponent<cbit::ecs::TextComponent>();
    localPlayerStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    localPlayerStatusText.fontSize = 18.0F;
    localPlayerStatusText.color = {235, 244, 255, 255};
    localPlayerStatusText.centered = false;
    _localPlayerStatusTextId = localPlayerStatus.getComponent<cbit::ecs::IdComponent>().id;

    auto opponentStatus = world.addGameObject("OpponentStatusText");
    opponentStatus.getComponent<cbit::ecs::TransformComponent>().position = {870.0F, 160.0F};
    auto& opponentStatusText = opponentStatus.addComponent<cbit::ecs::TextComponent>();
    opponentStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
    opponentStatusText.fontSize = 18.0F;
    opponentStatusText.color = {235, 244, 255, 255};
    opponentStatusText.centered = false;
    _opponentStatusTextId = opponentStatus.getComponent<cbit::ecs::IdComponent>().id;
}

/**
 * @brief Refreshes the HUD text from the current match state.
 */
void PlayScene::refreshMatchHud()
{
    if (auto phaseText = world.getGameObject(_phaseTextId)) {
        auto& text = phaseText.getComponent<cbit::ecs::TextComponent>();
        text.content = std::string("Phase: ") + gameplay::toString(_matchState.round.phase);
    }

    if (auto roundText = world.getGameObject(_roundTextId)) {
        auto& text = roundText.getComponent<cbit::ecs::TextComponent>();
        text.content = "Round: " + std::to_string(_matchState.round.roundNumber)
            + "/" + std::to_string(_matchState.maxRounds)
            + "  Players: " + std::to_string(_matchState.playerCount)
            + "  Undealt Cards: " + std::to_string(_matchState.round.shuffledDeck.size() - _matchState.round.nextDrawIndex);
    }

    if (_matchState.players.size() > 0) {
        if (auto localPlayerStatus = world.getGameObject(_localPlayerStatusTextId)) {
            const auto& player = _matchState.players[0];
            std::ostringstream builder;
            builder << player.displayName
                << "  Coins: " << player.coins
                << "  Bet: " << player.currentBet
                << "  Hand: " << (player.handCard ? player.handCard->definition.name : "none")
                << "  Head: " << (player.headCardRevealedToOwner && player.headCard ? player.headCard->definition.name : "hidden");
            localPlayerStatus.getComponent<cbit::ecs::TextComponent>().content = builder.str();
        }
    }

    if (_matchState.players.size() > 1) {
        if (auto opponentStatus = world.getGameObject(_opponentStatusTextId)) {
            const auto& player = _matchState.players[1];
            std::ostringstream builder;
            builder << player.displayName
                << "  Coins: " << player.coins
                << "  Bet: " << player.currentBet
                << "  Hand: " << (player.handCard ? "hidden" : "none")
                << "  Head: " << (player.headCard ? player.headCard->definition.name : "none");
            opponentStatus.getComponent<cbit::ecs::TextComponent>().content = builder.str();
        }
    }
}

/**
 * @brief Creates one draggable card entity.
 * @param cardInstance Card instance used for the spawned card.
 * @param position Spawn position.
 */
void PlayScene::createCard(const gameplay::CardInstance& cardInstance, const glm::vec2& position)
{
    auto card = world.addGameObject(cardInstance.definition.name);
    auto& transform = card.getComponent<cbit::ecs::TransformComponent>();
    transform.position = position;

    auto& cardComponent = card.addComponent<CardComponent>();
    cardComponent.name = cardInstance.definition.name;
    cardComponent.value = cardInstance.definition.scoreValue;
    cardComponent.faceUp = false;
    cardComponent.frontSourcePosition = cardInstance.definition.sourcePosition;

    auto& sprite = card.addComponent<cbit::ecs::SpriteComponent>();
    sprite.assetPath = kCardTexturePath;
    sprite.textureSize = {588.0F, 240.0F};
    sprite.sourcePosition = kCardBackSourcePosition;
    sprite.sourceSize = {kCardWidth, kCardHeight};
    sprite.size = {kCardWidth * 1.5F, kCardHeight * 1.5F};
    sprite.renderOrder = 1;

    auto& collider = card.addComponent<cbit::ecs::ColliderComponent>();
    collider.size = sprite.size;

    card.addComponent<cbit::ecs::DragableComponent>();
}

/**
 * @brief Creates a visual slot on the board.
 * @param tag Readable slot label.
 * @param position Slot center position.
 * @param kind Logical slot role used for snap behavior.
 */
void PlayScene::createSlot(std::string_view tag, const glm::vec2& position, const SlotKind kind)
{
    auto slot = world.addGameObject(std::string(tag));
    auto& transform = slot.getComponent<cbit::ecs::TransformComponent>();
    transform.position = position;

    auto& sprite = slot.addComponent<cbit::ecs::SpriteComponent>();
    sprite.assetPath = kSquareSlotTexturePath;
    sprite.textureSize = {128.0F, 128.0F};
    sprite.sourcePosition = {0.0F, 0.0F};
    sprite.sourceSize = {128.0F, 128.0F};
    sprite.size = {kCardWidth + kSlotSizePadding, kCardHeight + kSlotSizePadding};
    sprite.renderOrder = 0;

    auto& slotComponent = slot.addComponent<SlotComponent>();
    slotComponent.kind = kind;
}

} // namespace shark_card_game::scenes
