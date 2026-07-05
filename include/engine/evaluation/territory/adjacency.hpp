#pragma once

#include "engine/core/bitboard.hpp"

namespace engine::evaluation {

[[nodiscard]] BitBoard adjacent_squares(BitBoard squares) noexcept;

}
