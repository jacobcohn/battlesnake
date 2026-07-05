#include "engine/evaluation/territory/frontiers.hpp"

#include <cassert>
#include <cstddef>

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

}
