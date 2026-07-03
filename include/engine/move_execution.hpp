#pragma once

#include <array>
#include <cstdint>

#include "bitboard.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "position.hpp"
#include "snake.hpp"

struct Undo {
  std::array<Snake, MAX_SNAKES> snakes{};

  BitBoard occupied;
  BitBoard food;
  BitBoard hazards;

  uint8_t snake_count = 0;
  uint8_t you = 0;
  uint16_t turn = 0;

  uint64_t exact_hash = 0;
  uint64_t space_hash = 0;
};

void make_move(Position& position, const JointMove& joint, Undo& undo) noexcept;
void unmake_move(Position& position, const Undo& undo) noexcept;
