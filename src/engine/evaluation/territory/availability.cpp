#include "engine/evaluation/territory/availability.hpp"

#include <cassert>
#include <cstddef>

namespace engine::evaluation {

TerritoryAvailability::TerritoryAvailability() noexcept {
  next_release_square.fill(INVALID_SQUARE);
}

TerritoryAvailability initialize_availability(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  TerritoryAvailability availability{};
  availability.available &= ~position.occupied;

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    availability.next_release_square[static_cast<std::size_t>(snake_index)] = snake.tail;
  }

  return availability;
}

void release_body_squares(const Position& position, TerritoryAvailability& availability) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    Square& next_release_square = availability.next_release_square[static_cast<std::size_t>(snake_index)];
    if (next_release_square == INVALID_SQUARE) {
      continue;
    }

    assert(next_release_square >= 0 && next_release_square < BOARD_SQUARES);
    if (next_release_square < 0 || next_release_square >= BOARD_SQUARES) {
      next_release_square = INVALID_SQUARE;
      continue;
    }

    const Square released_square = next_release_square;
    availability.available.set(released_square);

    if (released_square == snake.head) {
      next_release_square = INVALID_SQUARE;
    } else {
      next_release_square = snake.next[static_cast<std::size_t>(released_square)];
    }
  }
}

}
