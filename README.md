# Shark Card Game

Shark Card Game is a multiplayer bluffing and betting card game built with `Cbit2d`.

The long-term target is a game for `2` to `4` players. Right now, the other players are intended to be NPC-controlled. Later, the same game flow can be extended to online multiplayer so real players can join over the internet.

## Game Overview

Each player starts with:

- `100` coins
- two card slots:
  - `hand`
  - `head`

At the start of each round, all cards are placed randomly in the middle of the table, and the players sit around the table.

Each player draws:

- `1` card into the `hand` slot
- `1` card into the `head` slot

Visibility rules:

- a player can see their own `hand` card
- a player cannot see their own `head` card
- other players can see that player's `head` card

This makes the game a mix of probability, deduction, and bluffing.

## Round Flow

Each match lasts `4` rounds.

### 1. Deal Phase

- shuffle the deck
- place cards at the center of the table
- each player draws `2` cards:
  - `1` for `hand`
  - `1` for `head`

### 2. Betting Phase

During betting, each player decides how many coins to bet based on:

- the card in their own `hand`
- the visible `head` cards of the other players
- their guess about the hidden `head` card on themselves

Choices during betting:

- bet some amount of coins
- refuse to bet

The intention is that confident players bet more, while uncertain players can stay conservative.

### 3. Reveal Phase

Once every player has finished betting:

- all players reveal their own `head` card
- each player adds:
  - `hand` card
  - `head` card

The player with the highest total wins the round.

### 4. Round Result

If a player wins the round:

- they receive double their own bet
- they also collect the coins bet by the other players

If a player loses the round:

- they lose the coins they bet

### 5. Match Result

After `4` rounds:

- total remaining coins are calculated for every player
- the player with the highest coin total wins the game

## Planned Player Count

- minimum: `2` players
- maximum: `4` players

Current expected development order:

1. local player vs NPC opponents
2. better NPC behavior and betting logic
3. support for more player counts
4. networked multiplayer for real players

## Current Repository Status

The project is currently being migrated to the latest `Cbit2d` API.

What already exists:

- top-level build wired to `../Cbit2d`
- scene-based startup with `MenuScene` and `PlayScene`
- placeholder play table with draggable cards and slot snapping

What is not finished yet:

- actual round state and turn sequencing
- dealing cards to per-player hand/head ownership
- hidden-information rendering rules
- betting logic
- reveal and scoring logic
- NPC decision making
- multiplayer support

## Implementation Plan

The clean way to achieve the full gameplay is to build it in layers.

### Phase 1: Round State Model

Add explicit gameplay state for:

- players
- player coins
- round number
- deck state
- hand card per player
- head card per player
- current bets
- folded / no-bet state
- revealed / hidden card state

Recommended approach:

- keep round and match state in plain C++ gameplay structs
- use ECS mainly for visuals, interaction, and UI
- avoid putting all game rules directly into rendering components

### Phase 2: Player Representation

Create a clear player model for up to `4` players:

- local human player
- `1` to `3` NPC players for now

Each player should track:

- display name
- seat position
- current coins
- current hand card
- current head card
- current bet
- active / folded state
- NPC or human control mode

### Phase 3: Deal Phase

Implement deterministic round setup:

1. shuffle deck
2. assign one hand card and one head card to each player
3. place those cards into the correct scene slots
4. apply visibility rules:
   - local player hand is visible
   - local player head is hidden from the local player
   - other players' head cards are visible

For the first playable version, use one local player plus one NPC.

### Phase 4: Betting Phase

Add turn-based betting UI and state transitions.

Needed systems:

- current active player
- minimum and maximum legal bet
- confirm bet action
- refuse-to-bet action
- pot tracking

UI needed:

- player coin display
- current round display
- current bet display
- action buttons for bet / pass / confirm

NPC behavior can start simple:

- estimate confidence from visible information
- choose from a few fixed bet bands
- sometimes refuse to bet

### Phase 5: Reveal And Winner Resolution

After betting ends:

1. reveal all hidden head cards
2. calculate each player's total
3. determine round winner
4. update coins
5. advance to the next round or finish the match

You should also define tie behavior explicitly. Recommended first rule:

- if totals tie, split the pot evenly among tied winners

### Phase 6: Match Flow

Build the full match controller:

- start match
- play `4` rounds
- show round summary after each round
- show final winner after round `4`
- allow returning to menu and restarting

### Phase 7: NPC Logic

Once the base rules work, improve NPCs with:

- difficulty levels
- bluffing behavior
- betting based on visible head cards
- conservative or aggressive personalities

### Phase 8: Online Multiplayer

After local rules are stable:

- separate gameplay state from rendering and local input
- make the match state serializable
- define a network-safe turn protocol
- add client/server or host/client architecture
- replace NPC players with remote players when connected

This should happen only after the local game rules are stable.

## Recommended Next Coding Steps

The most practical immediate next steps are:

1. replace the current free-form draggable prototype with explicit player seats and owned card slots
2. introduce gameplay structs for `PlayerState`, `RoundState`, and `MatchState`
3. implement a real `Deal Phase` for `2` players first
4. render hidden vs visible cards correctly based on the local player perspective
5. add a basic betting UI for one human player and one NPC
6. implement reveal, scoring, and round progression

## Notes

- The README describes the intended gameplay target, not a fully completed implementation.
- NPCs are the short-term substitute for missing multiplayer.
- Online multiplayer should reuse the same core game rules rather than creating a separate rules path.
