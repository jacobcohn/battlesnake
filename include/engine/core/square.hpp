#pragma once

#include <cassert>
#include <cstdint>

#include "constants.hpp"

using Square = int16_t;

inline constexpr Square INVALID_SQUARE = -1;

[[nodiscard]] constexpr Square idx(int x, int y) noexcept {
  assert(x >= 0 && x < BOARD_WIDTH);
  assert(y >= 0 && y < BOARD_HEIGHT);

  return static_cast<Square>(y * BOARD_WIDTH + x);
}

[[nodiscard]] constexpr int x_of(Square sq) noexcept {
  assert(sq >= 0 && sq < BOARD_SQUARES);

  return static_cast<int>(sq) % BOARD_WIDTH;
}

[[nodiscard]] constexpr int y_of(Square sq) noexcept {
  assert(sq >= 0 && sq < BOARD_SQUARES);

  return static_cast<int>(sq) / BOARD_WIDTH;
}
