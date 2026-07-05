#pragma once

#include "engine/core/move.hpp"
#include "engine/core/position.hpp"

namespace engine::search {

struct SearchLimits {
  int max_depth = 1;
  int time_limit_ms = 0;
};

struct SearchResult {
  JointMove best_move{};
  int score = 0;
  int depth = 0;
  bool completed = false;
};

[[nodiscard]] SearchResult search(Position& position, const SearchLimits& limits) noexcept;

}
