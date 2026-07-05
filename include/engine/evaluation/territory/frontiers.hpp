#pragma once

#include <array>

#include "engine/core/bitboard.hpp"
#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"
#include "engine/evaluation/territory/availability.hpp"

namespace engine::evaluation {

struct TerritoryFrontiers {
  std::array<BitBoard, MAX_SNAKES> current{};
  std::array<BitBoard, MAX_SNAKES> next{};
  std::array<BitBoard, MAX_SNAKES> reached{};
};

[[nodiscard]] TerritoryFrontiers initialize_frontiers(const Position& position) noexcept;

}
