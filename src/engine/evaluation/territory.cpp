#include "engine/evaluation/territory.hpp"

#include <array>
#include <cassert>
#include <cstddef>

namespace engine::evaluation {

namespace {

struct TerritoryAvailability {
  BitBoard available = BOARD_MASK;
  std::array<Square, MAX_SNAKES> next_release_square{};

  TerritoryAvailability() noexcept {
    next_release_square.fill(INVALID_SQUARE);
  }
};

struct TerritoryFrontiers {
  std::array<BitBoard, MAX_SNAKES> current{};
  std::array<BitBoard, MAX_SNAKES> next{};
  std::array<BitBoard, MAX_SNAKES> reached{};
};

struct TerritoryClaims {
  std::array<int, BOARD_SQUARES> snake{};
  std::array<int, BOARD_SQUARES> turn{};
  std::array<int, BOARD_SQUARES> length{};
  std::array<bool, BOARD_SQUARES> contested{};

  TerritoryClaims() noexcept {
    snake.fill(-1);
    turn.fill(BOARD_SQUARES * 2 + 1);
    length.fill(0);
    contested.fill(false);
  }
};

[[nodiscard]] TerritoryAvailability initialize_availability(const Position& position) noexcept {
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
  TerritoryAvailability availability = initialize_availability(position);
  TerritoryFrontiers frontiers = initialize_frontiers(position);
  TerritoryClaims claims{};

  // Heads are movement sources at turn 0, but they are not territory yet.
  // reached remains empty until a snake legally arrives on a later turn.
  release_body_squares(position, availability);

  (void)availability;
  (void)frontiers;
  (void)claims;
  return {};
}

}
