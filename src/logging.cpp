#include "logging.h"

#include <iostream>

void log_minimax_stats(Move selected_move, const MinimaxStats& minimax_stats) {
  for (const MinimaxDepthStats& depth_stats : minimax_stats.depth_stats) {
    std::cout << "minimax depth=" << depth_stats.depth
      << " selected=" << move_to_string(depth_stats.selected_move)
      << " elapsed_ms=" << depth_stats.elapsed_ms
      << " nodes=" << depth_stats.nodes_evaluated
      << " prunes=" << depth_stats.branches_pruned
      << std::endl;
  }

  std::cout << "minimax summary"
    << " selected=" << move_to_string(selected_move)
    << " completed_depth=" << minimax_stats.completed_depth
    << " elapsed_ms=" << minimax_stats.elapsed_ms
    << " timed_out=" << minimax_stats.timed_out
    << " nodes=" << minimax_stats.nodes_evaluated
    << " prunes=" << minimax_stats.branches_pruned
    << std::endl;
}
