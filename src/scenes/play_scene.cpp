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
#include <numbers>
#include <sstream>
#include <utility>
#include <entt/entity/registry.hpp>
#include "cbit/core/audio_service.hpp"
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
            return slotKind == SlotKind::PlayerHand || slotKind == SlotKind::NonPlayableCharacterHead;
        }

        /**
         * @brief Applies a simple ease-out curve for card travel.
         * @param t Interpolation factor in the range [0, 1].
         * @return Eased interpolation factor.
         */
        float easeOutCubic(const float t) {
            const float clamped = std::clamp(t, 0.0F, 1.0F);
            const float inverse = 1.0F - clamped;
            return 1.0F - (inverse * inverse * inverse);
        }

        /**
         * @brief Builds the short procedural sound used when a card is dealt.
         * @return Mono floating-point PCM samples for one deal tick.
         */
        std::vector<float> buildDealSoundBuffer() {
            constexpr int kSampleRate = 48000;
            constexpr float kDurationSeconds = 0.045F;
            constexpr float kFrequencyHz = 980.0F;
            const int sampleCount = static_cast<int>(kSampleRate * kDurationSeconds);

            std::vector<float> buffer(static_cast<std::size_t>(sampleCount));
            for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex) {
                const float time = static_cast<float>(sampleIndex) / static_cast<float>(kSampleRate);
                const float envelope = 1.0F - (static_cast<float>(sampleIndex) / static_cast<float>(sampleCount));
                buffer[static_cast<std::size_t>(sampleIndex)] =
                    std::sin(2.0F * std::numbers::pi_v<float> * kFrequencyHz * time) * envelope;
            }

            return buffer;
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
                "Opening cards are dealt automatically. Your hand is visible, your head is hidden, and NPC head cards are revealed.";
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
        dealOpeningCards();
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
        updateDealAnimation(deltaTimeSeconds);
        refreshMatchHud();
        world.update(deltaTimeSeconds);
    }

    /**
     * @brief Creates all static slot entities for the card table.
     */
    void PlayScene::createBoardSlots() {
        _handSlotIds.assign(_matchState.players.size(), 0);
        _headSlotIds.assign(_matchState.players.size(), 0);

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

            _handSlotIds[seatLayout.playerIndex] = createSlot(
                seatLayout.handTag,
                seatLayout.handPosition,
                seatLayout.handKind);
            _headSlotIds[seatLayout.playerIndex] = createSlot(
                seatLayout.headTag,
                seatLayout.headPosition,
                seatLayout.headKind);

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
        _deckCardIds.clear();
        _deckCardIds.reserve(_matchState.round.shuffledDeck.size());

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
     * @brief Prepares one hand card and one head card for each player.
     */
    void PlayScene::dealOpeningCards() {
        const std::size_t playerCount = _matchState.players.size();
        const std::size_t requiredCards = playerCount * 2;
        if (_deckCardIds.size() < requiredCards || _handSlotIds.size() < playerCount || _headSlotIds.size() < playerCount) {
            cbit2d::core::Logger::error("SharkCardGame could not deal opening cards because the scene state is incomplete");
            return;
        }

        _dealSteps.clear();
        _dealSteps.reserve(requiredCards);
        _nextDealStepIndex = 0;
        _isDealing = true;
        _dealStepDelayRemainingSeconds = 0.0F;
        _activeDealAnimation.cardId = 0;
        _activeDealAnimation.slotId = 0;
        _activeDealAnimation.elapsedSeconds = 0.0F;
        if (_dealSoundBuffer.empty()) {
            _dealSoundBuffer = buildDealSoundBuffer();
        }

        struct PreparedDealStep {
            cbit::ecs::GameObjectId handCardId = 0;
            cbit::ecs::GameObjectId handSlotId = 0;
            cbit::ecs::GameObjectId headCardId = 0;
            cbit::ecs::GameObjectId headSlotId = 0;
        };

        std::vector<PreparedDealStep> preparedSteps(playerCount);
        for (std::size_t playerIndex = 0; playerIndex < playerCount; ++playerIndex) {
            auto &player = _matchState.players[playerIndex];

            const std::size_t handCardIndex = _matchState.round.nextDrawIndex++;
            const std::size_t headCardIndex = _matchState.round.nextDrawIndex++;

            player.handCard = _matchState.round.shuffledDeck[handCardIndex];
            player.headCard = _matchState.round.shuffledDeck[headCardIndex];
            player.headCardRevealedToOwner = false;

            preparedSteps[playerIndex] = PreparedDealStep{
                _deckCardIds[handCardIndex],
                _handSlotIds[playerIndex],
                _deckCardIds[headCardIndex],
                _headSlotIds[playerIndex]
            };
        }

        const std::vector<int> clockwiseSeatOrder{1, 3, 0, 2};
        for (const int seatIndex: clockwiseSeatOrder) {
            if (seatIndex >= static_cast<int>(preparedSteps.size())) {
                continue;
            }

            const PreparedDealStep &preparedStep = preparedSteps[seatIndex];
            if (preparedStep.handCardId != 0 && preparedStep.handSlotId != 0) {
                _dealSteps.push_back(DealStep{preparedStep.handCardId, preparedStep.handSlotId});
            }
        }

        for (const int seatIndex: clockwiseSeatOrder) {
            if (seatIndex >= static_cast<int>(preparedSteps.size())) {
                continue;
            }

            const PreparedDealStep &preparedStep = preparedSteps[seatIndex];
            if (preparedStep.headCardId != 0 && preparedStep.headSlotId != 0) {
                _dealSteps.push_back(DealStep{preparedStep.headCardId, preparedStep.headSlotId});
            }
        }
    }

    /**
     * @brief Advances the opening deal animation.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void PlayScene::updateDealAnimation(const float deltaTimeSeconds) {
        if (!_isDealing) {
            return;
        }

        if (_activeDealAnimation.cardId != 0) {
            auto card = world.getGameObject(_activeDealAnimation.cardId);
            if (!card) {
                _activeDealAnimation.cardId = 0;
                _activeDealAnimation.slotId = 0;
                return;
            }

            _activeDealAnimation.elapsedSeconds += deltaTimeSeconds;
            const float normalizedTime = _dealTravelDurationSeconds > 0.0F
                ? _activeDealAnimation.elapsedSeconds / _dealTravelDurationSeconds
                : 1.0F;
            const float easedTime = easeOutCubic(normalizedTime);

            auto &transform = card.getComponent<cbit::ecs::TransformComponent>();
            transform.position = _activeDealAnimation.startPosition
                                 + ((_activeDealAnimation.targetPosition - _activeDealAnimation.startPosition)
                                    * easedTime);
            transform.position.y -= std::sin(std::numbers::pi_v<float> * std::clamp(normalizedTime, 0.0F, 1.0F))
                                    * _dealArcHeight;

            if (normalizedTime >= 1.0F) {
                placeCardInSlot(_activeDealAnimation.cardId, _activeDealAnimation.slotId);
                _activeDealAnimation.cardId = 0;
                _activeDealAnimation.slotId = 0;
                _activeDealAnimation.elapsedSeconds = 0.0F;
                _dealStepDelayRemainingSeconds = _dealStepDelaySeconds;
            }

            return;
        }

        if (_nextDealStepIndex >= _dealSteps.size()) {
            _isDealing = false;
            return;
        }

        _dealStepDelayRemainingSeconds -= deltaTimeSeconds;
        if (_dealStepDelayRemainingSeconds > 0.0F) {
            return;
        }

        beginNextDealStep();
    }

    /**
     * @brief Starts the next queued card animation if available.
     */
    void PlayScene::beginNextDealStep() {
        if (_nextDealStepIndex >= _dealSteps.size()) {
            _isDealing = false;
            return;
        }

        const DealStep &step = _dealSteps[_nextDealStepIndex++];
        auto card = world.getGameObject(step.cardId);
        auto slot = world.getGameObject(step.slotId);
        if (!card || !slot) {
            beginNextDealStep();
            return;
        }

        auto &cardTransform = card.getComponent<cbit::ecs::TransformComponent>();
        auto &cardData = card.getComponent<CardComponent>();
        auto &cardSprite = card.getComponent<cbit::ecs::SpriteComponent>();
        auto &dragable = card.getComponent<cbit::ecs::DragableComponent>();
        const auto &slotTransform = slot.getComponent<cbit::ecs::TransformComponent>();

        cardData.faceUp = false;
        cardData.snappedSlotId = 0;
        cardSprite.sourcePosition = kCardBackSourcePosition;
        dragable.enabled = false;
        dragable.isDragging = false;
        cbit2d::core::AudioService::playOneShot(_dealSoundBuffer, 0.18F);

        _activeDealAnimation.cardId = step.cardId;
        _activeDealAnimation.slotId = step.slotId;
        _activeDealAnimation.startPosition = cardTransform.position;
        _activeDealAnimation.targetPosition = slotTransform.position;
        _activeDealAnimation.elapsedSeconds = 0.0F;
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

        _deckCardIds.push_back(card.getComponent<cbit::ecs::IdComponent>().id);

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

        auto &dragable = card.addComponent<cbit::ecs::DragableComponent>();
        dragable.enabled = false;
    }

    /**
     * @brief Places one card entity into a specific slot and applies visibility.
     * @param cardId Card entity to place.
     * @param slotId Target slot entity.
     */
    void PlayScene::placeCardInSlot(const cbit::ecs::GameObjectId cardId, const cbit::ecs::GameObjectId slotId) {
        auto card = world.getGameObject(cardId);
        auto slot = world.getGameObject(slotId);
        if (!card || !slot) {
            return;
        }

        const auto &slotTransform = slot.getComponent<cbit::ecs::TransformComponent>();
        const auto &slotData = slot.getComponent<SlotComponent>();

        auto &cardTransform = card.getComponent<cbit::ecs::TransformComponent>();
        auto &cardSprite = card.getComponent<cbit::ecs::SpriteComponent>();
        auto &cardData = card.getComponent<CardComponent>();
        auto &dragable = card.getComponent<cbit::ecs::DragableComponent>();

        cardTransform.position = slotTransform.position;
        cardData.faceUp = shouldShowFrontFace(slotData.kind);
        cardData.snappedSlotId = slotId;
        cardSprite.sourcePosition = cardData.faceUp ? cardData.frontSourcePosition : kCardBackSourcePosition;
        dragable.enabled = false;
        dragable.isDragging = false;
    }

    /**
     * @brief Creates a visual slot on the board.
     * @param tag Readable slot label.
     * @param position Slot center position.
     * @param kind Logical slot role used for snap behavior.
     */
    cbit::ecs::GameObjectId PlayScene::createSlot(std::string_view tag, const glm::vec2 &position, const SlotKind kind) {
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

        return slot.getComponent<cbit::ecs::IdComponent>().id;
    }
} // namespace shark_card_game::scenes
