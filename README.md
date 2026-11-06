# BattleSnake Engine

A C++20 adversarial decision engine for [BattleSnake](https://play.battlesnake.com/), exposed through a minimal HTTP API using Crow.

The project is primarily an algorithms and systems-engineering exercise: it models simultaneous-move game states with compact board representations, applies deterministic move simulation and undo, evaluates territory control, and searches for robust moves under a strict per-turn time budget. It had previously reached 12th place on the Battlesnake Duels leaderboard.

## Competitive Results

- **Peak ranking:** 12th place on the Battlesnake Duels leaderboard.
- **Competition format:** Head-to-head games against other Battlesnake engines.
- **Current status:** No longer actively deployed or competing.

The engine achieved its peak ranking using adversarial minimax search, simultaneous-move simulation, and territory-control evaluation.

## Technical Highlights

- **C++20 engine implementation** with a small, explicit data model for game state.
- **Bitboard-based board representation** for an 11x11 Battlesnake board.
- **Simultaneous move simulation** for wall, body, health, food, tail, and head-to-head collision rules.
- **Make/unmake move system** to support deep search without repeatedly reallocating or rebuilding state.
- **Iterative deepening minimax search** with pruning-style cutoffs and a fixed move-time budget.
- **Territory-control evaluation** used as the non-terminal scoring heuristic.
- **Zobrist hashing infrastructure** for compact position identity and future transposition-table optimization.
- **Search diagnostics** returned with each move: score, depth, nodes, cutoffs, elapsed time, and timeout status.
- **Unit-tested core mechanics** covering board primitives, movement, hashing, evaluation, search, and request parsing.

## Architecture

The runtime flow is:

```text
Battlesnake request
  -> JSON parser
  -> internal Position representation
  -> move generation and search
  -> move response with diagnostics
```

Important modules:

```text
include/engine/core       Basic board, square, snake, move, and position types
src/engine/move           Move generation, application, collision handling, and undo
src/engine/evaluation     Static evaluation and territory-control logic
src/engine/hash           Zobrist hash computation and updates
src/engine/search         Iterative deepening minimax search
src/server                HTTP parsing, logging, and response helpers
src/main.cpp              Crow server entry point
```

## Search

On each `/move` request, the server parses the Battlesnake state into a compact `Position` and runs iterative deepening minimax search. Battlesnake is a simultaneous-move game, so the engine evaluates joint moves rather than alternating single-agent turns.

At each search node, the engine:

1. Generates all legal move combinations for the live snakes.
2. Groups outcomes by the controlled snake's candidate move.
3. Simulates each candidate against the opponent's possible simultaneous replies.
4. Scores the candidate by its worst-case opponent reply.
5. Selects the move with the best worst-case score.

This produces conservative decisions that prefer moves remaining strong under adversarial replies. Search uses make/unmake move operations so deeper plies can be explored without repeatedly copying full board state. The implementation also records node counts, cutoffs, depth reached, elapsed time, and timeout status for inspection.

## Evaluation

Terminal positions are scored directly as win, loss, or draw states, with earlier wins and later losses preferred.

Non-terminal positions are evaluated with a territory-control heuristic. Territory is computed by expanding bitboard frontiers outward from each snake's head over future turns:

- Occupied body squares are initially unavailable.
- Tail and body squares are released over time as snakes move.
- Squares are claimed by the snake that can reach them first.
- If snakes arrive at the same time, the longer snake controls the square.
- Equal-length simultaneous arrivals are treated as contested.

The final heuristic is the controlled snake's reachable territory minus the opponent's reachable territory. This encourages moves that preserve mobility, deny space, and avoid positions where the opponent can force a trap.

## Future Improvements

- Generalize the engine beyond two-snake games.
- Add fuller transposition-table integration and stronger move ordering.
- Improve evaluation speed, especially territory-control computation.
- Expand benchmarking around search depth, node count, and latency.

## Deployment

Deployment instructions are available in [`deployment.md`](deployment.md).

