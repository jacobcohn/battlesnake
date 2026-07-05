#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "engine/constants.hpp"
#include "engine/square.hpp"

enum class Move : uint8_t {
  Up,
  Down,
  Left,
  Right,
};

struct JointMove {
  std::array<Move, MAX_SNAKES> moves{};
};

namespace move_internal {

inline constexpr std::size_t MOVE_COUNT = 4;

[[nodiscard]] constexpr std::size_t move_index(Move move) noexcept {
  return static_cast<std::size_t>(move);
}

[[nodiscard]] constexpr std::array<std::array<Square, MOVE_COUNT>, BOARD_SQUARES>
make_move_targets() noexcept {
  std::array<std::array<Square, MOVE_COUNT>, BOARD_SQUARES> targets{};

  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      const Square from = idx(x, y);
      targets[static_cast<std::size_t>(from)][move_index(Move::Up)] =
          y + 1 < BOARD_HEIGHT ? idx(x, y + 1) : INVALID_SQUARE;
      targets[static_cast<std::size_t>(from)][move_index(Move::Down)] =
          y > 0 ? idx(x, y - 1) : INVALID_SQUARE;
      targets[static_cast<std::size_t>(from)][move_index(Move::Left)] =
          x > 0 ? idx(x - 1, y) : INVALID_SQUARE;
      targets[static_cast<std::size_t>(from)][move_index(Move::Right)] =
          x + 1 < BOARD_WIDTH ? idx(x + 1, y) : INVALID_SQUARE;
    }
  }

  return targets;
}

inline constexpr auto MOVE_TARGETS = make_move_targets();

}

[[nodiscard]] constexpr Square move_target(Square from, Move move) noexcept {
  if (from == INVALID_SQUARE) {
    return INVALID_SQUARE;
  }

  assert(from >= 0 && from < BOARD_SQUARES);
  return move_internal::MOVE_TARGETS[static_cast<std::size_t>(from)][move_internal::move_index(move)];
}
