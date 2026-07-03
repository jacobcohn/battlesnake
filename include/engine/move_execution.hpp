#pragma once

#include <array>
#include <cstdint>

#include "constants.hpp"
#include "move.hpp"
#include "position.hpp"
#include "square.hpp"

struct Undo {
  std::array<Square, MAX_SNAKES> old_heads{};
  std::array<Square, MAX_SNAKES> old_tails{};
  std::array<Square, MAX_SNAKES> new_heads{};
  std::array<uint8_t, MAX_SNAKES> old_health{};
  std::array<bool, MAX_SNAKES> ate_food{};
  std::array<bool, MAX_SNAKES> died{};

  constexpr Undo() noexcept {
    old_heads.fill(INVALID_SQUARE);
    old_tails.fill(INVALID_SQUARE);
    new_heads.fill(INVALID_SQUARE);
    old_health.fill(0);
    ate_food.fill(false);
    died.fill(false);
  }
};

[[nodiscard]] Undo make_move(Position& position, const JointMove& joint) noexcept;
void unmake_move(Position& position, const Undo& undo) noexcept;
