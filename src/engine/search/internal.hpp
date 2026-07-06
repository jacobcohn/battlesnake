#pragma once

#include <array>
#include <chrono>

#include "engine/search/search.hpp"
#include "engine/search/transposition_table.hpp"

namespace engine::search::internal {

inline constexpr int INF = 1000000000;

inline constexpr std::array<Move, 4> ALL_MOVES{
    Move::Up,
    Move::Down,
    Move::Left,
    Move::Right,
};

struct SearchContext {
  std::chrono::steady_clock::time_point deadline{};
  TranspositionTable transposition_table{};
  uint64_t nodes = 0;
  uint64_t cutoffs = 0;
  uint64_t tt_probes = 0;
  uint64_t tt_hits = 0;
  uint64_t tt_cutoffs = 0;
  uint64_t tt_stores = 0;
  bool has_deadline = false;
  bool stopped = false;
};

struct SearchValue {
  int score = 0;
  bool completed = true;
};

[[nodiscard]] SearchContext make_context(const SearchLimits& limits) noexcept;
[[nodiscard]] bool should_stop(SearchContext& context) noexcept;

[[nodiscard]] bool terminal(const Position& position) noexcept;

[[nodiscard]] SearchResult fallback_result(const Position& position) noexcept;

[[nodiscard]] SearchValue search_value(
    Position& position,
    int depth,
    int alpha,
    int beta,
    SearchContext& context) noexcept;

[[nodiscard]] SearchResult search_root(Position& position, int depth, SearchContext& context) noexcept;

}
