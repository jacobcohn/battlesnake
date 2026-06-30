#pragma once

#include <chrono>
#include <vector>

#include "evaluation/params.h"
#include "game_state.h"
#include "play/move.h"

struct MinimaxDepthStats {
  int depth = 0;
  Move selected_move = Move::Up;
  int nodes_evaluated = 0;
  int branches_pruned = 0;
  int elapsed_ms = 0;
};

struct MinimaxStats {
  int completed_depth = 0;
  int nodes_evaluated = 0;
  int branches_pruned = 0;
  int elapsed_ms = 0;
  bool timed_out = false;
  std::vector<MinimaxDepthStats> depth_stats;
};

Move choose_minimax_move(
  const GameState& state,
  std::chrono::milliseconds latency_budget,
  const EvaluationParams& params,
  MinimaxStats& stats);

Move choose_minimax_move(
  const GameState& state,
  std::chrono::milliseconds latency_budget,
  MinimaxStats& stats);
