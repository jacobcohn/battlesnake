#pragma once

#include <array>
#include <cstdint>

#include "engine/core/constants.hpp"

enum class Move : uint8_t {
  Up,
  Down,
  Left,
  Right,
};

struct JointMove {
  std::array<Move, MAX_SNAKES> moves{};
};

[[nodiscard]] constexpr const char* move_name(Move move) noexcept {
  switch (move) {
    case Move::Up:
      return "up";
    case Move::Down:
      return "down";
    case Move::Left:
      return "left";
    case Move::Right:
      return "right";
  }

  return "up";
}
