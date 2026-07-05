#pragma once

#include <array>

#include "engine/core/bitboard.hpp"
#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"
#include "engine/core/square.hpp"

namespace engine::evaluation {

struct TerritoryAvailability {
  BitBoard available = BOARD_MASK;
  std::array<Square, MAX_SNAKES> next_release_square{};

  TerritoryAvailability() noexcept;
};

[[nodiscard]] TerritoryAvailability initialize_availability(const Position& position) noexcept;
void release_body_squares(const Position& position, TerritoryAvailability& availability) noexcept;

}
