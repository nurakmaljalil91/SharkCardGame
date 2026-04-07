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
#include <limits>
#include <numbers>
#include <sstream>
#include <utility>

#include <entt/entity/registry.hpp>

#include "cbit/core/audio_service.hpp"
#include "cbit/core/input.hpp"
#include "cbit/core/logger.hpp"
#include "cbit/ecs/components.hpp"
#include "cbit/ecs/entity_component_system.hpp"
#include "table_layout.h"

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
        createBettingPanel();
        createMatchFinishedPanel();
        createBoardSlots();
        createDeck();
        dealOpeningCards();
        refreshBettingPanel();
        refreshMatchFinishedPanel();
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
        updateBettingPhase(deltaTimeSeconds);
        updateRevealAndResolution(deltaTimeSeconds);
        refreshBettingPanel();
        refreshMatchFinishedPanel();
        refreshMatchHud();
        world.update(deltaTimeSeconds);
    }

    /**
     * @brief Creates all static slot entities for the card table.
     */
    void PlayScene::createBoardSlots() {
        _handSlotIds.assign(_matchState.players.size(), 0);
        _headSlotIds.assign(_matchState.players.size(), 0);
        _handCardIds.assign(_matchState.players.size(), 0);
        _headCardIds.assign(_matchState.players.size(), 0);

        const std::vector<SeatLayout> seatLayouts = buildSeatLayouts(_matchState);

        for (const SeatLayout &seatLayout: seatLayouts) {
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
        _matchState.round.phase = gameplay::MatchPhase::Deal;
        _matchState.round.pot = 0;
        _matchState.round.playersActedCount = 0;
        _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
        _selectedBetAmount = 0;
        _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
        _revealDelayRemainingSeconds = _revealDelaySeconds;
        _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
        _roundResolved = false;
        _winningTotal = 0;
        _winningSeatIndices.clear();
        _roundResultSummary.clear();
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
            player.currentBet = 0;
            player.hasBetThisRound = false;
            player.declinedBet = false;
            player.headCardRevealedToOwner = false;

            preparedSteps[playerIndex] = PreparedDealStep{
                _deckCardIds[handCardIndex],
                _handSlotIds[playerIndex],
                _deckCardIds[headCardIndex],
                _headSlotIds[playerIndex]
            };
            _handCardIds[playerIndex] = _deckCardIds[handCardIndex];
            _headCardIds[playerIndex] = _deckCardIds[headCardIndex];
        }

        const std::vector<int> clockwiseSeatOrder = buildDealSeatOrder(_matchState);
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
            _matchState.round.phase = gameplay::MatchPhase::Betting;
            _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
            _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
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
        _ui.create(
            world,
            _matchState,
            [this]() { selectBetOption(-1); },
            [this]() { selectBetOption(5); },
            [this]() { selectBetOption(10); },
            [this]() { selectBetOption(20); },
            [this]() { confirmLocalPlayerBet(); },
            [this]() { startNewGame(); }
        );
    }

    /**
     * @brief Refreshes the HUD text from the current match state.
     */
    void PlayScene::refreshMatchHud() {
        _ui.refresh(world, PlaySceneUiModel{
            _matchState,
            _isDealing,
            _selectedBetAmount,
            _winningSeatIndices,
            _roundResultSummary
        });
    }

    /**
     * @brief Creates the placeholder betting panel for the upcoming betting phase.
     */
    void PlayScene::createBettingPanel() {
    }

    /**
     * @brief Creates the centered match-finished overlay panel.
     */
    void PlayScene::createMatchFinishedPanel() {
    }

    /**
     * @brief Refreshes betting panel text, styling, and button state.
     */
    void PlayScene::refreshBettingPanel() {
        _ui.refresh(world, PlaySceneUiModel{
            _matchState,
            _isDealing,
            _selectedBetAmount,
            _winningSeatIndices,
            _roundResultSummary
        });
    }

    /**
     * @brief Refreshes the match-finished overlay visibility and text.
     */
    void PlayScene::refreshMatchFinishedPanel() {
    }

    /**
     * @brief Advances simple NPC betting turns.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void PlayScene::updateBettingPhase(const float deltaTimeSeconds) {
        if (_isDealing || _matchState.round.phase != gameplay::MatchPhase::Betting) {
            return;
        }

        if (_matchState.round.activePlayerSeatIndex == _matchState.localPlayerSeatIndex) {
            return;
        }

        _npcBetDelayRemainingSeconds -= deltaTimeSeconds;
        if (_npcBetDelayRemainingSeconds > 0.0F) {
            return;
        }

        const auto &activePlayer = _matchState.players[static_cast<std::size_t>(_matchState.round.activePlayerSeatIndex)];
        const int handValue = activePlayer.handCard ? activePlayer.handCard->definition.scoreValue : 0;
        int desiredBet = 0;
        if (handValue >= 11) {
            desiredBet = 20;
        } else if (handValue >= 7) {
            desiredBet = 10;
        } else if (handValue >= 4) {
            desiredBet = 5;
        }

        commitBetForActivePlayer(desiredBet);
    }

    /**
     * @brief Selects one local-player betting option.
     * @param amount Selected bet amount, or `-1` for pass.
     */
    void PlayScene::selectBetOption(const int amount) {
        if (_isDealing
            || _matchState.round.phase != gameplay::MatchPhase::Betting
            || _matchState.round.activePlayerSeatIndex != _matchState.localPlayerSeatIndex) {
            return;
        }

        _selectedBetAmount = amount;
    }

    /**
     * @brief Commits the local player's currently selected betting choice.
     */
    void PlayScene::confirmLocalPlayerBet() {
        if (_selectedBetAmount == 0) {
            return;
        }

        if (_matchState.round.activePlayerSeatIndex != _matchState.localPlayerSeatIndex) {
            return;
        }

        commitBetForActivePlayer(_selectedBetAmount < 0 ? 0 : _selectedBetAmount);
    }

    /**
     * @brief Applies one betting decision to the current active player.
     * @param amount Bet amount, or `0` to pass.
     */
    void PlayScene::commitBetForActivePlayer(const int amount) {
        if (_matchState.round.phase != gameplay::MatchPhase::Betting) {
            return;
        }

        gameplay::applyBetChoice(_matchState, _matchState.round.activePlayerSeatIndex, amount);

        if (_matchState.round.activePlayerSeatIndex == _matchState.localPlayerSeatIndex) {
            _selectedBetAmount = 0;
        }

        ++_matchState.round.playersActedCount;
        advanceBettingTurn();
    }

    /**
     * @brief Advances betting to the next player or phase.
     */
    void PlayScene::advanceBettingTurn() {
        if (_matchState.round.playersActedCount >= static_cast<int>(_matchState.players.size())) {
            _matchState.round.phase = gameplay::MatchPhase::Reveal;
            _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
            _revealDelayRemainingSeconds = _revealDelaySeconds;
            _roundResolved = false;
            revealAllHeadCards();
            return;
        }

        _matchState.round.activePlayerSeatIndex =
            (_matchState.round.activePlayerSeatIndex + 1) % static_cast<int>(_matchState.players.size());
        _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
    }

    /**
     * @brief Advances reveal and round-resolution state.
     * @param deltaTimeSeconds Elapsed time since the previous frame.
     */
    void PlayScene::updateRevealAndResolution(const float deltaTimeSeconds) {
        if (_matchState.round.phase == gameplay::MatchPhase::Reveal && !_roundResolved) {
            _revealDelayRemainingSeconds -= deltaTimeSeconds;
            if (_revealDelayRemainingSeconds <= 0.0F) {
                resolveRoundResult();
            }
            return;
        }

        if (_matchState.round.phase == gameplay::MatchPhase::RoundResolution && _roundResolved) {
            _roundResolutionDelayRemainingSeconds -= deltaTimeSeconds;
            if (_roundResolutionDelayRemainingSeconds <= 0.0F) {
                advanceRoundFlow();
            }
        }
    }

    /**
     * @brief Reveals all head cards for the current round.
     */
    void PlayScene::revealAllHeadCards() {
        for (std::size_t playerIndex = 0; playerIndex < _matchState.players.size(); ++playerIndex) {
            auto &player = _matchState.players[playerIndex];
            player.headCardRevealedToOwner = true;

            if (playerIndex < _handCardIds.size()) {
                if (auto handCard = world.getGameObject(_handCardIds[playerIndex])) {
                    auto &handCardData = handCard.getComponent<CardComponent>();
                    auto &handCardSprite = handCard.getComponent<cbit::ecs::SpriteComponent>();
                    handCardData.faceUp = true;
                    handCardSprite.sourcePosition = handCardData.frontSourcePosition;
                }
            }

            if (playerIndex >= _headCardIds.size()) {
                continue;
            }

            auto card = world.getGameObject(_headCardIds[playerIndex]);
            if (!card) {
                continue;
            }

            auto &cardData = card.getComponent<CardComponent>();
            auto &cardSprite = card.getComponent<cbit::ecs::SpriteComponent>();
            cardData.faceUp = true;
            cardSprite.sourcePosition = cardData.frontSourcePosition;
        }
    }

    /**
     * @brief Computes winners and resolves round payouts.
     */
    void PlayScene::resolveRoundResult() {
        const gameplay::RoundResolution resolution = gameplay::resolveRound(_matchState);
        _winningSeatIndices = resolution.winningSeatIndices;
        _winningTotal = resolution.winningTotal;
        _roundResultSummary = resolution.summary;

        _roundResolved = true;
        _matchState.round.phase = gameplay::MatchPhase::RoundResolution;
        _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
    }

    /**
     * @brief Removes all round card entities from the scene.
     */
    void PlayScene::clearRoundCards() {
        const auto removeCardIds = [this](std::vector<cbit::ecs::GameObjectId> &cardIds) {
            for (const cbit::ecs::GameObjectId cardId: cardIds) {
                if (auto card = world.getGameObject(cardId)) {
                    world.removeGameObject(card);
                }
            }

            cardIds.assign(_matchState.players.size(), 0);
        };

        removeCardIds(_handCardIds);
        removeCardIds(_headCardIds);
        _dealSteps.clear();
        _nextDealStepIndex = 0;
        _activeDealAnimation = ActiveDealAnimation{};
    }

    /**
     * @brief Starts the next round or finishes the match if all rounds are complete.
     */
    void PlayScene::advanceRoundFlow() {
        if (_matchState.round.roundNumber >= _matchState.maxRounds) {
            int bestCoins = std::numeric_limits<int>::min();
            _winningSeatIndices.clear();

            for (std::size_t playerIndex = 0; playerIndex < _matchState.players.size(); ++playerIndex) {
                const int coins = _matchState.players[playerIndex].coins;
                if (_winningSeatIndices.empty() || coins > bestCoins) {
                    _winningSeatIndices = {static_cast<int>(playerIndex)};
                    bestCoins = coins;
                } else if (coins == bestCoins) {
                    _winningSeatIndices.push_back(static_cast<int>(playerIndex));
                }
            }

            std::ostringstream builder;
            if (_winningSeatIndices.size() == 1) {
                builder << _matchState.players[static_cast<std::size_t>(_winningSeatIndices.front())].displayName
                        << " wins the match with " << bestCoins << " coins";
            } else {
                builder << "Match tied at " << bestCoins << " coins between ";
                for (std::size_t winnerIndex = 0; winnerIndex < _winningSeatIndices.size(); ++winnerIndex) {
                    if (winnerIndex > 0) {
                        builder << ", ";
                    }

                    builder << _matchState.players[static_cast<std::size_t>(_winningSeatIndices[winnerIndex])].displayName;
                }
            }

            _roundResultSummary = builder.str();
            _matchState.round.phase = gameplay::MatchPhase::MatchFinished;
            _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
            return;
        }

        clearRoundCards();
        ++_matchState.round.roundNumber;
        _matchState.round.phase = gameplay::MatchPhase::Deal;
        _matchState.round.pot = 0;
        _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
        _matchState.round.playersActedCount = 0;
        layoutRemainingDeck();
        _matchState.round.activePlayerSeatIndex = _matchState.localPlayerSeatIndex;
        dealOpeningCards();
    }

    /**
     * @brief Resets the current scene into a fresh new match.
     */
    void PlayScene::startNewGame() {
        clearRoundCards();

        for (std::size_t cardIndex = _matchState.round.nextDrawIndex; cardIndex < _deckCardIds.size(); ++cardIndex) {
            if (auto card = world.getGameObject(_deckCardIds[cardIndex])) {
                world.removeGameObject(card);
            }
        }

        _deckCardIds.clear();
        _matchState = gameplay::createInitialMatchState(_matchState.playerCount, _matchState.localPlayerSeatIndex);
        _selectedBetAmount = 0;
        _npcBetDelayRemainingSeconds = _npcBetDelaySeconds;
        _revealDelayRemainingSeconds = _revealDelaySeconds;
        _roundResolutionDelayRemainingSeconds = _roundResolutionDelaySeconds;
        _roundResolved = false;
        _winningTotal = 0;
        _winningSeatIndices.clear();
        _roundResultSummary.clear();

        createDeck();
        dealOpeningCards();
    }

    /**
     * @brief Reflows the remaining undealt cards into a compact deck grid.
     */
    void PlayScene::layoutRemainingDeck() {
        std::size_t remainingIndex = 0;
        for (std::size_t cardIndex = _matchState.round.nextDrawIndex;
             cardIndex < _deckCardIds.size();
             ++cardIndex, ++remainingIndex) {
            const cbit::ecs::GameObjectId cardId = _deckCardIds[cardIndex];
            auto card = world.getGameObject(cardId);
            if (!card) {
                continue;
            }

            const float column = static_cast<float>(remainingIndex % 13);
            const float row = static_cast<float>(remainingIndex / 13);
            auto &transform = card.getComponent<cbit::ecs::TransformComponent>();
            auto &cardData = card.getComponent<CardComponent>();
            auto &sprite = card.getComponent<cbit::ecs::SpriteComponent>();
            auto &dragable = card.getComponent<cbit::ecs::DragableComponent>();

            transform.position = {
                _deckOrigin.x + (column * (kCardWidth + kCardSpacing)),
                _deckOrigin.y + (row * (kCardHeight + kCardSpacing))
            };
            transform.rotation = 0.0F;
            cardData.faceUp = false;
            cardData.snappedSlotId = 0;
            sprite.sourcePosition = kCardBackSourcePosition;
            dragable.enabled = false;
            dragable.isDragging = false;
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
