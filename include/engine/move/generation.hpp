#pragma once

#include <array>

#include "engine/core/constants.hpp"
#include "engine/core/move.hpp"
#include "engine/core/position.hpp"

namespace engine::move {

[[nodiscard]] constexpr int max_joint_moves() noexcept {
  int result = 1;
  for (int i = 0; i < MAX_SNAKES; ++i) {
    result *= 4;
  }

  return result;
}

inline constexpr int MAX_JOINT_MOVES = max_joint_moves();

struct JointMoveList {
  std::array<JointMove, MAX_JOINT_MOVES> moves{};
  int count = 0;
};

[[nodiscard]] const JointMoveList& generate_joint_moves(const Position& position) noexcept;

}
