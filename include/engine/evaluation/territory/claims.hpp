#pragma once

#include <array>

#include "engine/core/constants.hpp"

namespace engine::evaluation {

struct TerritoryClaims {
  std::array<int, BOARD_SQUARES> snake{};
  std::array<int, BOARD_SQUARES> turn{};
  std::array<int, BOARD_SQUARES> length{};
  std::array<bool, BOARD_SQUARES> contested{};

  TerritoryClaims() noexcept;
};

}
