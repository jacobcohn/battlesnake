#include "engine/evaluation/territory.hpp"

#include <array>
#include <cassert>
#include <cstddef>

namespace engine::evaluation {

namespace {

using AvailableTurns = std::array<int, BOARD_SQUARES>;

struct TerritoryFrontiers {
  std::array<BitBoard, MAX_SNAKES> current{};
  std::array<BitBoard, MAX_SNAKES> next{};
  std::array<BitBoard, MAX_SNAKES> reached{};
};

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

[[nodiscard]] TerritoryFrontiers initialize_frontiers(const Position& position) noexcept {
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

TerritoryControl compute_territory_control(const Position& position) noexcept {
  const AvailableTurns available_turns = build_available_turns(position);
  TerritoryFrontiers frontiers = initialize_frontiers(position);

  // Heads are movement sources at turn 0, but they are not territory yet.
  // reached remains empty until a snake legally arrives on a later turn.
  (void)available_turns;
  (void)frontiers;
  return {};
}

}
