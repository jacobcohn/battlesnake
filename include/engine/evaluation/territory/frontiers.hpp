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

void expand_frontiers(
    const Position& position,
    const TerritoryAvailability& availability,
    TerritoryFrontiers& frontiers
) noexcept;

[[nodiscard]] bool next_frontiers_empty(
    const Position& position,
    const TerritoryFrontiers& frontiers
) noexcept;

void advance_frontiers(TerritoryFrontiers& frontiers) noexcept;

}
