#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "bitboard.hpp"
#include "constants.hpp"
#include "snake.hpp"

struct Position {
  std::array<Snake, MAX_SNAKES> snakes{};

  BitBoard occupied;
  BitBoard food;

  uint8_t snake_count = 0;
  uint8_t you = 0;
  uint16_t turn = 0;

  uint64_t exact_hash = 0;
  uint64_t space_hash = 0;

  constexpr void reset() noexcept {
    for (Snake& snake : snakes) {
      snake.reset();
    }

    clear_bitboards();

    snake_count = 0;
    you = 0;
    turn = 0;

    exact_hash = 0;
    space_hash = 0;
  }

  constexpr void clear_bitboards() noexcept {
    occupied = BitBoard{};
    food = BitBoard{};
  }

  constexpr void rebuild_occupied() noexcept {
    assert(snake_count <= MAX_SNAKES);
    occupied = BitBoard{};

    for (int i = 0; i < snake_count; ++i) {
      const Snake& snake = snakes[static_cast<std::size_t>(i)];

      if (!snake.alive || snake.length == 0) {
        continue;
      }

      Square cur = snake.tail;
      for (int visited = 0; cur != INVALID_SQUARE && visited < snake.length; ++visited) {
        occupied.set(cur);
        cur = snake.next[static_cast<std::size_t>(cur)];
      }
    }
  }
};
