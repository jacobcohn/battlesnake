#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "bitboard.hpp"
#include "constants.hpp"
#include "snake.hpp"

struct Position {
  std::array<Snake, MAX_SNAKES> snakes{};
  BitBoard occupied;
  BitBoard food;
  BitBoard hazards;
  uint8_t snakeCount = 0;
  uint8_t you = 0;
  uint16_t turn = 0;
  uint64_t exactHash = 0;
  uint64_t spaceHash = 0;

  constexpr void reset() noexcept {
    for (Snake& snake : snakes) {
      snake.reset();
    }
    clearBitboards();
    snakeCount = 0;
    you = 0;
    turn = 0;
    exactHash = 0;
    spaceHash = 0;
  }

  constexpr void clearBitboards() noexcept {
    occupied = BitBoard{};
    food = BitBoard{};
    hazards = BitBoard{};
  }

  constexpr void rebuildOccupied() noexcept {
    occupied = BitBoard{};

    const uint8_t count = snakeCount > MAX_SNAKES ? MAX_SNAKES : snakeCount;
    for (uint8_t i = 0; i < count; ++i) {
      const Snake& snake = snakes[i];
      if (!snake.alive || snake.length == 0) {
        continue;
      }

      Square cur = snake.tail;
      for (uint8_t visited = 0; cur != INVALID_SQUARE && visited < snake.length; ++visited) {
        occupied.set(cur);
        cur = snake.next[static_cast<std::size_t>(cur)];
      }
    }
  }
};
