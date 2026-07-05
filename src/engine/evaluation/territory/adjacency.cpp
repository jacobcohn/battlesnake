#include "engine/evaluation/territory/adjacency.hpp"

#include <array>
#include <cstddef>

#include "engine/core/constants.hpp"
#include "engine/core/square.hpp"

namespace engine::evaluation {

namespace {

[[nodiscard]] std::array<BitBoard, BOARD_SQUARES> build_adjacent_masks() noexcept {
  std::array<BitBoard, BOARD_SQUARES> masks{};

  for (Square square = 0; square < BOARD_SQUARES; ++square) {
    const int x = x_of(square);
    const int y = y_of(square);

    if (x > 0) {
      masks[static_cast<std::size_t>(square)].set(idx(x - 1, y));
    }
    if (x + 1 < BOARD_WIDTH) {
      masks[static_cast<std::size_t>(square)].set(idx(x + 1, y));
    }
    if (y > 0) {
      masks[static_cast<std::size_t>(square)].set(idx(x, y - 1));
    }
    if (y + 1 < BOARD_HEIGHT) {
      masks[static_cast<std::size_t>(square)].set(idx(x, y + 1));
    }
  }

  return masks;
}

[[nodiscard]] const std::array<BitBoard, BOARD_SQUARES>& adjacent_masks() noexcept {
  static const std::array<BitBoard, BOARD_SQUARES> masks = build_adjacent_masks();
  return masks;
}

}

BitBoard adjacent_squares(BitBoard squares) noexcept {
  const std::array<BitBoard, BOARD_SQUARES>& masks = adjacent_masks();
  BitBoard adjacent{};

  for (Square square = 0; square < BOARD_SQUARES; ++square) {
    if (squares.test(square)) {
      adjacent |= masks[static_cast<std::size_t>(square)];
    }
  }

  return adjacent;
}

}
