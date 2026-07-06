#include "engine/evaluation/territory/claims.hpp"

#include <cassert>
#include <cstddef>

namespace engine::evaluation {

namespace {

constexpr int UNREACHED_TURN = BOARD_SQUARES * 2 + 1;
constexpr int NO_SNAKE = -1;

void resolve_claim(
    const Snake& snake,
    int snake_index,
    TerritoryClaims& claims,
    Square square,
    int turn
) noexcept {
  const std::size_t square_index = static_cast<std::size_t>(square);

  if (turn < claims.turn[square_index]) {
    claims.snake[square_index] = snake_index;
    claims.turn[square_index] = turn;
    claims.length[square_index] = snake.length;
    claims.contested[square_index] = false;
    return;
  }

  if (turn != claims.turn[square_index]) {
    return;
  }

  if (snake.length > claims.length[square_index]) {
    claims.snake[square_index] = snake_index;
    claims.length[square_index] = snake.length;
    claims.contested[square_index] = false;
    return;
  }

  if (snake.length == claims.length[square_index]) {
    claims.snake[square_index] = NO_SNAKE;
    claims.contested[square_index] = true;
  }
}

}

TerritoryClaims::TerritoryClaims() noexcept {
  snake.fill(NO_SNAKE);
  turn.fill(UNREACHED_TURN);
  length.fill(0);
  contested.fill(false);
}

TerritoryClaims initialize_claims() noexcept {
  return TerritoryClaims{};
}

void record_arrivals(
    const Position& position,
    TerritoryFrontiers& frontiers,
    TerritoryClaims& claims,
    int turn
) noexcept {
  assert(position.snake_count <= MAX_SNAKES);
  assert(turn > 0);

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    BitBoard& reached = frontiers.reached[static_cast<std::size_t>(snake_index)];
    const BitBoard& next = frontiers.next[static_cast<std::size_t>(snake_index)];
    BitBoard arrivals = next & ~reached;

    reached |= arrivals;

    Square square = INVALID_SQUARE;
    while (arrivals.pop_first_set_square(square)) {
      resolve_claim(snake, snake_index, claims, square, turn);
    }
  }
}

bool all_squares_resolved(const TerritoryClaims& claims) noexcept {
  for (Square square = 0; square < BOARD_SQUARES; ++square) {
    if (claims.turn[static_cast<std::size_t>(square)] == UNREACHED_TURN) {
      return false;
    }
  }

  return true;
}

TerritoryControl count_control(
    const Position& position,
    const TerritoryClaims& claims
) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  TerritoryControl control{};

  for (Square square = 0; square < BOARD_SQUARES; ++square) {
    const std::size_t square_index = static_cast<std::size_t>(square);
    const int snake_index = claims.snake[square_index];

    if (snake_index == NO_SNAKE || claims.contested[square_index]) {
      continue;
    }

    assert(snake_index >= 0 && snake_index < position.snake_count);
    if (snake_index < 0 || snake_index >= position.snake_count) {
      continue;
    }

    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    ++control[static_cast<std::size_t>(snake_index)];
  }

  return control;
}

}
