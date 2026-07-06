#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "bitboard.hpp"
#include "constants.hpp"
#include "square.hpp"

struct Snake {
  Square head = INVALID_SQUARE;
  Square tail = INVALID_SQUARE;
  std::array<Square, BOARD_SQUARES> next{};
  BitBoard occupied;
  uint64_t body_hash = 0;
  uint8_t length = 0;
  uint8_t stacked_tail_segments = 0;
  uint8_t health = 100;
  bool alive = false;

  constexpr Snake() noexcept {
    reset();
  }

  constexpr void reset() noexcept {
    head = INVALID_SQUARE;
    tail = INVALID_SQUARE;
    next.fill(INVALID_SQUARE);
    occupied = BitBoard{};
    body_hash = 0;
    length = 0;
    stacked_tail_segments = 0;
    health = 100;
    alive = false;
  }
};
