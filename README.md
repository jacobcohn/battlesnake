# Battlesnake Engine

A C++20 adversarial decision engine for [Battlesnake](https://play.battlesnake.com/), exposed through a minimal HTTP API using Crow.

The project is primarily an algorithms and systems-engineering exercise: it models simultaneous-move game states with compact board representations, applies deterministic move simulation and undo, evaluates territory control, and searches for robust moves under a strict per-turn time budget.

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
include/engine/core        Basic board, square, snake, move, and position types
src/engine/move           Move generation, application, collision handling, and undo
src/engine/evaluation     Static evaluation and territory-control logic
src/engine/hash           Zobrist hash computation and updates
src/engine/search         Iterative deepening minimax search
src/server                HTTP parsing, logging, and response helpers
src/main.cpp              Crow server entry point
```

## Search and Evaluation

On each `/move` request, the server parses the Battlesnake state into a compact `Position`, then runs iterative deepening search with a time limit. The search evaluates each candidate move against the opponent's possible simultaneous replies and chooses the move with the best worst-case outcome.

Terminal positions are scored as win/loss/draw states. Non-terminal positions are evaluated using territory control, rewarding reachable space for the controlled snake and penalizing opponent-controlled space.

## Future Improvements

- Generalize the engine beyond two-snake games.
- Add fuller transposition-table integration and stronger move ordering.
- Improve evaluation speed, especially territory-control computation.
- Expand benchmarking around search depth, node count, and latency.

## Deployment

Deployment instructions are available in [`deployment.md`](deployment.md).

