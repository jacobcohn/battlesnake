#pragma once

#include <array>

#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"
#include "engine/evaluation/territory/frontiers.hpp"
#include "engine/evaluation/territory/territory.hpp"

namespace engine::evaluation {

struct TerritoryClaims {
  std::array<int, BOARD_SQUARES> snake{};
  std::array<int, BOARD_SQUARES> turn{};
  std::array<int, BOARD_SQUARES> length{};
  std::array<bool, BOARD_SQUARES> contested{};

  TerritoryClaims() noexcept;
};

[[nodiscard]] TerritoryClaims initialize_claims() noexcept;

void record_arrivals(
    const Position& position,
    TerritoryFrontiers& frontiers,
    TerritoryClaims& claims,
    int turn
) noexcept;

[[nodiscard]] bool all_squares_resolved(const TerritoryClaims& claims) noexcept;

[[nodiscard]] TerritoryControl count_control(
    const Position& position,
    const TerritoryClaims& claims
) noexcept;

}
