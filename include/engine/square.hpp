#pragma once

#include <cstdint>

#include "constants.hpp"

using Square = int16_t;

inline constexpr Square INVALID_SQUARE = -1;

[[nodiscard]] constexpr int idx(int x, int y) noexcept {
  return y * BOARD_WIDTH + x;
}

[[nodiscard]] constexpr int x_of(Square sq) noexcept {
  return static_cast<int>(sq) % BOARD_WIDTH;
}

[[nodiscard]] constexpr int y_of(Square sq) noexcept {
  return static_cast<int>(sq) / BOARD_WIDTH;
}
