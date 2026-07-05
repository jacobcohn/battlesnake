#include "engine/evaluation/territory.hpp"

#include <array>
#include <cassert>

namespace engine::evaluation {

namespace {

using AvailableTurns = std::array<int, BOARD_SQUARES>;

[[nodiscard]] AvailableTurns build_available_turns(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  AvailableTurns available_turns{};

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    Square square = snake.tail;
    for (int release_turn = 1; release_turn <= snake.length; ++release_turn) {
      assert(square >= 0 && square < BOARD_SQUARES);
      if (square < 0 || square >= BOARD_SQUARES) {
        break;
      }

      if (release_turn > available_turns[static_cast<std::size_t>(square)]) {
        available_turns[static_cast<std::size_t>(square)] = release_turn;
      }

      if (square == snake.head) {
        break;
      }
      square = snake.next[static_cast<std::size_t>(square)];
    }
  }

  return available_turns;
}

}

TerritoryControl compute_territory_control(const Position& position) noexcept {
  const AvailableTurns available_turns = build_available_turns(position);
  (void)available_turns;
  return {};
}

}
