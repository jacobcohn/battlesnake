#include "engine/evaluation/territory/frontiers.hpp"

#include <cassert>
#include <cstddef>

#include "engine/evaluation/territory/adjacency.hpp"

namespace engine::evaluation {

TerritoryFrontiers initialize_frontiers(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  TerritoryFrontiers frontiers{};

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    assert(snake.head >= 0 && snake.head < BOARD_SQUARES);
    if (snake.head < 0 || snake.head >= BOARD_SQUARES) {
      continue;
    }

    frontiers.current[static_cast<std::size_t>(snake_index)].set(snake.head);
  }

  return frontiers;
}

void expand_frontiers(
    const Position& position,
    const TerritoryAvailability& availability,
    TerritoryFrontiers& frontiers
) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  for (BitBoard& next : frontiers.next) {
    next = BitBoard{};
  }

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    const BitBoard& current = frontiers.current[static_cast<std::size_t>(snake_index)];
    frontiers.next[static_cast<std::size_t>(snake_index)] =
        adjacent_squares(current) & availability.available;
  }
}

bool next_frontiers_empty(
    const Position& position,
    const TerritoryFrontiers& frontiers
) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    if (!frontiers.next[static_cast<std::size_t>(snake_index)].empty()) {
      return false;
    }
  }

  return true;
}

void advance_frontiers(TerritoryFrontiers& frontiers) noexcept {
  for (std::size_t snake_index = 0; snake_index < MAX_SNAKES; ++snake_index) {
    frontiers.current[snake_index] = frontiers.next[snake_index];
    frontiers.next[snake_index] = BitBoard{};
  }
}

}
