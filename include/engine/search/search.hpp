#pragma once

#include <array>
#include <cstdint>

#include "engine/core/move.hpp"
#include "engine/core/position.hpp"

namespace engine::search {

struct SearchLimits {
  int max_depth = 1;
  int time_limit_ms = 0;
};

inline constexpr int MAX_RECORDED_DEPTHS = 64;

struct DepthInfo {
  int depth = 0;
  int score = 0;
  Move best_move = Move::Up;
  uint64_t nodes = 0;
  uint64_t cutoffs = 0;
  uint64_t elapsed_ms = 0;
  bool completed = false;
};

struct SearchResult {
  JointMove best_move{};
  int score = 0;
  int depth = 0;
  bool completed = false;
  bool timed_out = false;
  std::array<DepthInfo, MAX_RECORDED_DEPTHS> depths{};
  int depth_count = 0;
};

[[nodiscard]] SearchResult search(Position& position, const SearchLimits& limits) noexcept;

}
