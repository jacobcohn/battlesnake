#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "constants.hpp"
#include "square.hpp"

struct Snake {
  Square head = INVALID_SQUARE;
  Square tail = INVALID_SQUARE;
  std::array<Square, BOARD_SQUARES> next{};
  uint8_t length = 0;
  uint8_t health = 100;
  bool alive = false;

  constexpr Snake() noexcept {
    reset();
  }

  constexpr void reset() noexcept {
    head = INVALID_SQUARE;
    tail = INVALID_SQUARE;
    next.fill(INVALID_SQUARE);
    length = 0;
    health = 100;
    alive = false;
  }

  [[nodiscard]] constexpr bool contains(Square sq) const noexcept {
    if (sq == INVALID_SQUARE || length == 0) {
      return false;
    }

    Square cur = tail;
    for (uint8_t visited = 0; cur != INVALID_SQUARE && visited < length; ++visited) {
      if (cur == sq) {
        return true;
      }
      cur = next[static_cast<std::size_t>(cur)];
    }
    return false;
  }

  [[nodiscard]] std::vector<Square> bodySquares() const {
    std::vector<Square> squares;
    squares.reserve(length);

    Square cur = tail;
    for (uint8_t visited = 0; cur != INVALID_SQUARE && visited < length; ++visited) {
      squares.push_back(cur);
      cur = next[static_cast<std::size_t>(cur)];
    }
    return squares;
  }
};
