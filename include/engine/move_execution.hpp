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

  std::array<bool, MAX_SNAKES> alive_at_start{};
  std::array<Square, MAX_SNAKES> old_tails{};
  std::array<Square, MAX_SNAKES> new_heads{};
  std::array<bool, MAX_SNAKES> ate_food{};
  std::array<bool, MAX_SNAKES> dies{};

  uint8_t snake_count = 0;
  uint8_t you = 0;
  uint16_t turn = 0;

  uint64_t exact_hash = 0;
  uint64_t space_hash = 0;
};

void make_move(Position& position, const JointMove& joint, Undo& undo) noexcept;
void unmake_move(Position& position, const Undo& undo) noexcept;
