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
        constexpr glm::vec2 kCardBackSourcePosition{546.0F, 0.0F};
        constexpr auto kCardTexturePath = "resources/images/cardsLarge_tilemap.png";
        constexpr auto kSquareSlotTexturePath = "resources/images/button_square_flat.png";

        /**
         * @brief Returns whether a slot should display cards face up.
         * @param slotKind Logical slot role.
         * @return `true` when snapped, cards should show their front face.
         */
        bool shouldShowFrontFace(const SlotKind slotKind) {
            return slotKind == SlotKind::PlayerHand || slotKind == SlotKind::NonPlayableCharacterHand;
        }
    } // namespace

    /**
     * @brief Creates the play scene with a menu callback.
     * @param onReturnToMenu Callback invoked when the menu button is pressed.
     */
    PlayScene::PlayScene(std::function<void()> onReturnToMenu)
        : _onReturnToMenu(std::move(onReturnToMenu)),
          _matchState(gameplay::createInitialMatchState(4, 0)) {
    }

    /**
     * @brief Initializes the play scene entities.
     */
    void PlayScene::initialize() {
        constexpr float kWindowCenterX = 640.0F;

        auto title = world.addGameObject("PlayTitle");
        auto &titleTransform = title.getComponent<cbit::ecs::TransformComponent>();
        titleTransform.position = {kWindowCenterX, 36.0F};

        auto &titleText = title.addComponent<cbit::ecs::TextComponent>();
        titleText.content = "Shark Card Game";
        titleText.fontPath = "resources/fonts/Kenney_Future.ttf";
        titleText.fontSize = 28.0F;
        titleText.color = {244, 246, 255, 255};

        auto instructions = world.addGameObject("Instructions");
        auto &instructionsTransform = instructions.getComponent<cbit::ecs::TransformComponent>();
        instructionsTransform.position = {kWindowCenterX, 102.0F};

        auto &instructionsText = instructions.addComponent<cbit::ecs::TextComponent>();
        instructionsText.content =
                "Drag cards into the highlighted slots. Hand slots reveal cards, head slots keep them hidden.";
        instructionsText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        instructionsText.fontSize = 15.0F;
        instructionsText.color = {210, 226, 240, 255};

        auto menuButton = world.addGameObject("BackToMenuButton");
        auto &menuButtonTransform = menuButton.getComponent<cbit::ecs::TransformComponent>();
        menuButtonTransform.position = {1130.0F, 52.0F};

        auto &menuButtonComponent = menuButton.addComponent<cbit::ecs::ButtonComponent>();
        menuButtonComponent.size = {180.0F, 52.0F};
        menuButtonComponent.backgroundColor = {34, 45, 67, 255};
        menuButtonComponent.hoverColor = {48, 64, 93, 255};
        menuButtonComponent.pressedColor = {22, 31, 46, 255};
        menuButtonComponent.borderColor = {231, 207, 115, 255};
        menuButtonComponent.onClick = _onReturnToMenu;

        auto &menuButtonText = menuButton.addComponent<cbit::ecs::TextComponent>();
        menuButtonText.content = "Menu";
        menuButtonText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        menuButtonText.fontSize = 20.0F;
        menuButtonText.color = {255, 255, 255, 255};

        createMatchHud();
        createBoardSlots();
        createDeck();
        refreshMatchHud();

        world.addSystem([](cbit::ecs::EntityComponentSystem &ecs) {
            if (!cbit2d::core::Input::isMouseButtonReleased(cbit2d::core::MouseButton::Left)) {
                return;
            }

            auto &registry = ecs.getRegistry();
            auto slotView = registry.view<const cbit::ecs::TransformComponent, const SlotComponent>();
            auto cardView = registry.view<cbit::ecs::TransformComponent, cbit::ecs::SpriteComponent, CardComponent,
                cbit::ecs::DragableComponent>();

            for (const auto cardEntity: cardView) {
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
                for (const auto slotEntity: slotView) {
                    const auto &slotTransform = slotView.get<const cbit::ecs::TransformComponent>(slotEntity);
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

                const auto &slotTransform = slotView.get<const cbit::ecs::TransformComponent>(closestSlot);
                const auto &slot = slotView.get<const SlotComponent>(closestSlot);
                const auto &slotId = registry.get<cbit::ecs::IdComponent>(closestSlot);

                cardTransform.position = slotTransform.position;
                card.faceUp = shouldShowFrontFace(slot.kind);
                card.snappedSlotId = slotId.id;
                cardSprite.sourcePosition = card.faceUp ? card.frontSourcePosition : kCardBackSourcePosition;
            }
        });

        world.addSystem([this](cbit::ecs::EntityComponentSystem &ecs) {
            auto &registry = ecs.getRegistry();
            auto cardView = registry.view<cbit::ecs::TransformComponent, cbit::ecs::SpriteComponent, CardComponent,
                cbit::ecs::DragableComponent>();
            for (const auto entity: cardView) {
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
            for (const auto entity: cardView) {
                auto &dragable = cardView.get<cbit::ecs::DragableComponent>(entity);
                if (dragable.isDragging) {
                    continue;
                }

                auto &sprite = cardView.get<cbit::ecs::SpriteComponent>(entity);
                auto &transform = cardView.get<cbit::ecs::TransformComponent>(entity);
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
    void PlayScene::update(float deltaTimeSeconds) {
        _deltaTimeSeconds = deltaTimeSeconds;
        refreshMatchHud();
        world.update(deltaTimeSeconds);
    }

    /**
     * @brief Creates all static slot entities for the card table.
     */
    void PlayScene::createBoardSlots() {
        struct SeatLayout {
            std::string handTag;
            std::string headTag;
            glm::vec2 handPosition;
            glm::vec2 headPosition;
            glm::vec2 labelPosition;
            SlotKind handKind;
            SlotKind headKind;
            int playerIndex;
        };

        const std::vector<SeatLayout> seatLayouts{
            {
                "Npc1HandSlot",
                "Npc1HeadSlot",
                {590.0F, 176.0F},
                {690.0F, 176.0F},
                {640.0F, 124.0F},
                SlotKind::NonPlayableCharacterHand,
                SlotKind::NonPlayableCharacterHead,
                1
            },
            {
                "Npc2HandSlot",
                "Npc2HeadSlot",
                {160.0F, 390.0F},
                {260.0F, 390.0F},
                {210.0F, 332.0F},
                SlotKind::NonPlayableCharacterHand,
                SlotKind::NonPlayableCharacterHead,
                2
            },
            {
                "Npc3HandSlot",
                "Npc3HeadSlot",
                {1020.0F, 390.0F},
                {1120.0F, 390.0F},
                {1070.0F, 332.0F},
                SlotKind::NonPlayableCharacterHand,
                SlotKind::NonPlayableCharacterHead,
                3
            },
            {
                "PlayerHandSlot",
                "PlayerHeadSlot",
                {590.0F, 614.0F},
                {690.0F, 614.0F},
                {640.0F, 674.0F},
                SlotKind::PlayerHand,
                SlotKind::PlayerHead,
                0
            }
        };

        for (const SeatLayout &seatLayout: seatLayouts) {
            if (seatLayout.playerIndex >= static_cast<int>(_matchState.players.size())) {
                continue;
            }

            createSlot(seatLayout.handTag, seatLayout.handPosition, seatLayout.handKind);
            createSlot(seatLayout.headTag, seatLayout.headPosition, seatLayout.headKind);

            auto label = world.addGameObject(seatLayout.handTag + "Label");
            label.getComponent<cbit::ecs::TransformComponent>().position = seatLayout.labelPosition;

            auto &labelText = label.addComponent<cbit::ecs::TextComponent>();
            labelText.content = _matchState.players[seatLayout.playerIndex].displayName;
            labelText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
            labelText.fontSize = 20.0F;
            labelText.color = {255, 255, 255, 255};
        }
    }

    /**
     * @brief Creates the deck of draggable cards.
     */
    void PlayScene::createDeck() {
        _deckOrigin = {
            (1280.0F - ((kCardWidth * 13.0F) + (kCardSpacing * 12.0F))) * 0.5F + (kCardWidth * 0.5F),
            290.0F
        };

        for (std::size_t index = 0; index < _matchState.round.shuffledDeck.size(); ++index) {
            const float column = static_cast<float>(index % 13);
            const float row = static_cast<float>(index / 13);
            const glm::vec2 position{
                _deckOrigin.x + (column * (kCardWidth + kCardSpacing)),
                _deckOrigin.y + (row * (kCardHeight + kCardSpacing))
            };
            createCard(_matchState.round.shuffledDeck[index], position);
        }
    }

    /**
     * @brief Creates HUD text that reflects the current match state.
     */
    void PlayScene::createMatchHud() {
        auto phaseText = world.addGameObject("PhaseInfoText");
        phaseText.getComponent<cbit::ecs::TransformComponent>().position = {72.0F, 38.0F};
        auto &phaseTextComponent = phaseText.addComponent<cbit::ecs::TextComponent>();
        phaseTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        phaseTextComponent.fontSize = 20.0F;
        phaseTextComponent.color = {249, 214, 119, 255};
        phaseTextComponent.centered = false;
        _phaseTextId = phaseText.getComponent<cbit::ecs::IdComponent>().id;

        auto roundText = world.addGameObject("RoundInfoText");
        roundText.getComponent<cbit::ecs::TransformComponent>().position = {72.0F, 64.0F};
        auto &roundTextComponent = roundText.addComponent<cbit::ecs::TextComponent>();
        roundTextComponent.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        roundTextComponent.fontSize = 16.0F;
        roundTextComponent.color = {214, 228, 240, 255};
        roundTextComponent.centered = false;
        _roundTextId = roundText.getComponent<cbit::ecs::IdComponent>().id;

        auto localPlayerStatus = world.addGameObject("LocalPlayerStatusText");
        localPlayerStatus.getComponent<cbit::ecs::TransformComponent>().position = {828.0F, 678.0F};
        auto &localPlayerStatusText = localPlayerStatus.addComponent<cbit::ecs::TextComponent>();
        localPlayerStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
        localPlayerStatusText.fontSize = 16.0F;
        localPlayerStatusText.color = {235, 244, 255, 255};
        localPlayerStatusText.centered = false;
        _localPlayerStatusTextId = localPlayerStatus.getComponent<cbit::ecs::IdComponent>().id;

        _opponentStatusTextIds.clear();
        for (std::size_t playerIndex = 1; playerIndex < _matchState.players.size(); ++playerIndex) {
            auto opponentStatus = world.addGameObject(
                "OpponentStatusText" + std::to_string(playerIndex));
            opponentStatus.getComponent<cbit::ecs::TransformComponent>().position = {
                820.0F,
                168.0F + (static_cast<float>(playerIndex - 1) * 28.0F)
            };

            auto &opponentStatusText = opponentStatus.addComponent<cbit::ecs::TextComponent>();
            opponentStatusText.fontPath = "resources/fonts/Kenney_Future_Narrow.ttf";
            opponentStatusText.fontSize = 16.0F;
            opponentStatusText.color = {235, 244, 255, 255};
            opponentStatusText.centered = false;
            _opponentStatusTextIds.push_back(opponentStatus.getComponent<cbit::ecs::IdComponent>().id);
        }
    }

    /**
     * @brief Refreshes the HUD text from the current match state.
     */
    void PlayScene::refreshMatchHud() {
        if (auto phaseText = world.getGameObject(_phaseTextId)) {
            auto &text = phaseText.getComponent<cbit::ecs::TextComponent>();
            text.content = std::string("Phase: ") + gameplay::toString(_matchState.round.phase);
        }

        if (auto roundText = world.getGameObject(_roundTextId)) {
            auto &text = roundText.getComponent<cbit::ecs::TextComponent>();
            text.content = "Round: " + std::to_string(_matchState.round.roundNumber)
                           + "/" + std::to_string(_matchState.maxRounds)
                           + "  Players: " + std::to_string(_matchState.playerCount)
                           + "  Undealt Cards: " + std::to_string(
                               _matchState.round.shuffledDeck.size() - _matchState.round.nextDrawIndex);
        }

        if (_matchState.players.size() > 0) {
            if (auto localPlayerStatus = world.getGameObject(_localPlayerStatusTextId)) {
                const auto &player = _matchState.players[0];
                std::ostringstream builder;
                builder << player.displayName
                        << "  Coins: " << player.coins
                        << "  Bet: " << player.currentBet
                        << "  Hand: " << (player.handCard ? player.handCard->definition.name : "none")
                        << "  Head: " << (player.headCardRevealedToOwner && player.headCard
                                              ? player.headCard->definition.name
                                              : "hidden");
                localPlayerStatus.getComponent<cbit::ecs::TextComponent>().content = builder.str();
            }
        }

        for (std::size_t playerIndex = 1; playerIndex < _matchState.players.size(); ++playerIndex) {
            const std::size_t statusIndex = playerIndex - 1;
            if (statusIndex >= _opponentStatusTextIds.size()) {
                continue;
            }

            if (auto opponentStatus = world.getGameObject(_opponentStatusTextIds[statusIndex])) {
                const auto &player = _matchState.players[playerIndex];
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
    void PlayScene::createCard(const gameplay::CardInstance &cardInstance, const glm::vec2 &position) {
        auto card = world.addGameObject(cardInstance.definition.name);
        auto &transform = card.getComponent<cbit::ecs::TransformComponent>();
        transform.position = position;

        auto &cardComponent = card.addComponent<CardComponent>();
        cardComponent.name = cardInstance.definition.name;
        cardComponent.value = cardInstance.definition.scoreValue;
        cardComponent.faceUp = false;
        cardComponent.frontSourcePosition = cardInstance.definition.sourcePosition;

        auto &sprite = card.addComponent<cbit::ecs::SpriteComponent>();
        sprite.assetPath = kCardTexturePath;
        sprite.textureSize = {588.0F, 240.0F};
        sprite.sourcePosition = kCardBackSourcePosition;
        sprite.sourceSize = {kCardWidth, kCardHeight};
        sprite.size = {kCardWidth * 1.5F, kCardHeight * 1.5F};
        sprite.renderOrder = 1;

        auto &collider = card.addComponent<cbit::ecs::ColliderComponent>();
        collider.size = sprite.size;

        card.addComponent<cbit::ecs::DragableComponent>();
    }

    /**
     * @brief Creates a visual slot on the board.
     * @param tag Readable slot label.
     * @param position Slot center position.
     * @param kind Logical slot role used for snap behavior.
     */
    void PlayScene::createSlot(std::string_view tag, const glm::vec2 &position, const SlotKind kind) {
        auto slot = world.addGameObject(std::string(tag));
        auto &transform = slot.getComponent<cbit::ecs::TransformComponent>();
        transform.position = position;

        auto &sprite = slot.addComponent<cbit::ecs::SpriteComponent>();
        sprite.assetPath = kSquareSlotTexturePath;
        sprite.textureSize = {128.0F, 128.0F};
        sprite.sourcePosition = {0.0F, 0.0F};
        sprite.sourceSize = {128.0F, 128.0F};
        sprite.size = {kCardWidth + kSlotSizePadding, kCardHeight + kSlotSizePadding};
        sprite.renderOrder = 0;

        auto &slotComponent = slot.addComponent<SlotComponent>();
        slotComponent.kind = kind;
    }
} // namespace shark_card_game::scenes
