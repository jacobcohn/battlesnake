#include "engine/search/search.hpp"

#include <chrono>
#include <cstddef>

#include "internal.hpp"

namespace engine::search {

namespace {

[[nodiscard]] uint64_t elapsed_ms(std::chrono::steady_clock::time_point start) noexcept {
  const std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
  const std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
  return static_cast<uint64_t>(elapsed.count());
}

void append_depth_info(SearchResult& result, const DepthInfo& depth_info) noexcept {
  if (result.depth_count >= MAX_RECORDED_DEPTHS) {
    return;
  }

  result.depths[static_cast<std::size_t>(result.depth_count)] = depth_info;
  ++result.depth_count;
}

}

SearchResult search(Position& position, const SearchLimits& limits) noexcept {
  SearchResult result = internal::fallback_result(position);
  const int max_depth = limits.max_depth > 0 ? limits.max_depth : 0;

  if (max_depth == 0) {
    result.completed = true;
    return result;
  }

  internal::SearchContext context = internal::make_context(limits);

  for (int depth = 1; depth <= max_depth; ++depth) {
    const uint64_t nodes_before = context.nodes;
    const uint64_t cutoffs_before = context.cutoffs;
    const uint64_t tt_probes_before = context.tt_probes;
    const uint64_t tt_hits_before = context.tt_hits;
    const uint64_t tt_cutoffs_before = context.tt_cutoffs;
    const uint64_t tt_stores_before = context.tt_stores;
    const std::chrono::steady_clock::time_point depth_start = std::chrono::steady_clock::now();

    SearchResult depth_result = internal::search_root(position, depth, context);

    DepthInfo depth_info;
    depth_info.depth = depth;
    depth_info.nodes = context.nodes - nodes_before;
    depth_info.cutoffs = context.cutoffs - cutoffs_before;
    depth_info.tt_probes = context.tt_probes - tt_probes_before;
    depth_info.tt_hits = context.tt_hits - tt_hits_before;
    depth_info.tt_cutoffs = context.tt_cutoffs - tt_cutoffs_before;
    depth_info.tt_stores = context.tt_stores - tt_stores_before;
    depth_info.elapsed_ms = elapsed_ms(depth_start);
    depth_info.completed = depth_result.completed;

    if (depth_result.completed) {
      depth_info.score = depth_result.score;
      depth_info.best_move = depth_result.best_move.moves[static_cast<std::size_t>(position.you)];
      result.best_move = depth_result.best_move;
      result.score = depth_result.score;
      result.depth = depth;
      result.completed = depth == max_depth;
    }

    append_depth_info(result, depth_info);

    if (!depth_result.completed) {
      result.completed = false;
      result.timed_out = context.stopped;
      return result;
    }
  }

  result.completed = true;
  result.timed_out = context.stopped;
  return result;
}

}
