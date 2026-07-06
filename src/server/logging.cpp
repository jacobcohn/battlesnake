#include "server/logging.hpp"

#include <cstddef>
#include <iostream>

#include "engine/core/move.hpp"

void log_search_result(const Position& position, const engine::search::SearchResult& result) noexcept {
  const char* selected_move = move_name(result.best_move.moves[static_cast<std::size_t>(position.you)]);

  std::clog << "search"
            << " turn=" << position.turn
            << " move=" << selected_move
            << " score=" << result.score
            << " depth=" << result.depth
            << " completed=" << result.completed
            << " timed_out=" << result.timed_out
            << '\n';

  for (int i = 0; i < result.depth_count; ++i) {
    const engine::search::DepthInfo& depth = result.depths[static_cast<std::size_t>(i)];
    std::clog << "search_depth"
              << " turn=" << position.turn
              << " depth=" << depth.depth
              << " completed=" << depth.completed
              << " nodes=" << depth.nodes
              << " cutoffs=" << depth.cutoffs
              << " elapsed_ms=" << depth.elapsed_ms;
    if (depth.completed) {
      std::clog << " move=" << move_name(depth.best_move)
                << " score=" << depth.score;
    }
    std::clog << '\n';
  }
}
