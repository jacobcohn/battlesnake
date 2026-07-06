#include "engine/evaluation/territory/claims.hpp"

#include <cassert>
#include <cstddef>

namespace engine::evaluation {

namespace {

constexpr int UNREACHED_TURN = BOARD_SQUARES * 2 + 1;
constexpr int NO_SNAKE = -1;

void remove_control(TerritoryClaims& claims, int snake_index) noexcept {
  if (snake_index < 0 || snake_index >= MAX_SNAKES) {
    return;
  }

  --claims.control[static_cast<std::size_t>(snake_index)];
}

void add_control(TerritoryClaims& claims, int snake_index) noexcept {
  if (snake_index < 0 || snake_index >= MAX_SNAKES) {
    return;
  }

  ++claims.control[static_cast<std::size_t>(snake_index)];
}

void clear_existing_control(TerritoryClaims& claims, std::size_t square_index) noexcept {
  if (claims.contested[square_index]) {
    return;
  }

  remove_control(claims, claims.snake[square_index]);
}

void resolve_claim(
    const Snake& snake,
    int snake_index,
    TerritoryClaims& claims,
    Square square,
    int turn
) noexcept {
  const std::size_t square_index = static_cast<std::size_t>(square);

  if (turn < claims.turn[square_index]) {
    if (claims.turn[square_index] == UNREACHED_TURN) {
      --claims.unresolved_count;
    } else {
      clear_existing_control(claims, square_index);
    }

    claims.snake[square_index] = snake_index;
    claims.turn[square_index] = turn;
    claims.length[square_index] = snake.length;
    claims.contested[square_index] = false;
    add_control(claims, snake_index);
    return;
  }

  if (turn != claims.turn[square_index]) {
    return;
  }

  if (snake.length > claims.length[square_index]) {
    clear_existing_control(claims, square_index);
    claims.snake[square_index] = snake_index;
    claims.length[square_index] = snake.length;
    claims.contested[square_index] = false;
    add_control(claims, snake_index);
    return;
  }

  if (snake.length == claims.length[square_index]) {
    clear_existing_control(claims, square_index);
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
  control.fill(0);
  unresolved_count = BOARD_SQUARES;
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
  return claims.unresolved_count == 0;
}

TerritoryControl count_control(
    const Position& position,
    const TerritoryClaims& claims
) noexcept {
  assert(position.snake_count <= MAX_SNAKES);
  return claims.control;
}

}
