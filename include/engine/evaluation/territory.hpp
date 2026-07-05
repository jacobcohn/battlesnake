#pragma once

#include <array>

#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"

namespace engine::evaluation {

using TerritoryControl = std::array<int, MAX_SNAKES>;

[[nodiscard]] TerritoryControl compute_territory_control(const Position& position) noexcept;

}
