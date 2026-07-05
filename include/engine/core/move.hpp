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
