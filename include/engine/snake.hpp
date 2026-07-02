#pragma once

#include <array>
#include <cassert>
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
};
