#pragma once

#include <cstdint>
#include <vector>

#include "game_state.h"
#include "move.h"

struct MinimaxMoveScore {
  Move move;
  int score = 0;
};

struct MinimaxStats {
  int depth = 0;
  std::int64_t nodes_evaluated = 0;
  std::int64_t elapsed_ms = 0;
  std::vector<MinimaxMoveScore> top_level_scores;
};

Move choose_minimax_move(const GameState& state, int depth, MinimaxStats& stats);
