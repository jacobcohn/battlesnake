#pragma once

#include "engine/core/position.hpp"

namespace engine::evaluation {

inline constexpr int WIN_SCORE = 1000000;
inline constexpr int DRAW_SCORE = -900000;
inline constexpr int LOSS_SCORE = -1000000;

[[nodiscard]] int evaluate(const Position& position) noexcept;

}
