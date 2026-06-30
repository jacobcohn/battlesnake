#include "minimax.h"

#include <algorithm>
#include <chrono>
#include <limits>

#include "evaluation/evaluate.h"
#include "simulate.h"

namespace {

constexpr int MAX_ITERATIVE_DEEPENING_DEPTH = 64;

struct BestMoveResult {
  Move selected_move = Move::Up;
  int score = std::numeric_limits<int>::min();
};

bool search_deadline_expired(std::chrono::steady_clock::time_point deadline) {
  return std::chrono::steady_clock::now() >= deadline;
}

BestMoveResult search_best_you_move(const GameState& state, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats);
int score_best_you_move(const GameState& state, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats);

int score_position(const GameState& state, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  if (search_deadline_expired(deadline)) {
    return std::numeric_limits<int>::min();
  }

  if (depth <= 0 || !state.you.alive || !state.opponent.alive) {
    ++stats.nodes_evaluated;
    return evaluate_state(state, params);
  }

  return score_best_you_move(state, depth, alpha, beta, deadline, params, stats);
}

int score_worst_opponent_response(const GameState& state, Move you_move, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  int worst_score = std::numeric_limits<int>::max();

  for (Move opponent_move : ALL_MOVES) {
    if (search_deadline_expired(deadline)) {
      return worst_score;
    }

    GameState next_state = simulate_turn(state, you_move, opponent_move);

    int score = score_position(next_state, depth - 1, alpha, beta, deadline, params, stats);
    worst_score = std::min(worst_score, score);

    beta = std::min(beta, worst_score);
    if (beta <= alpha) {
      ++stats.branches_pruned;
      break;
    }
  }

  return worst_score;
}

BestMoveResult search_best_you_move(const GameState& state, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  BestMoveResult result;

  for (Move you_move : ALL_MOVES) {
    if (search_deadline_expired(deadline)) {
      return result;
    }

    int score = score_worst_opponent_response(state, you_move, depth, alpha, beta, deadline, params, stats);
    if (score > result.score) {
      result.score = score;
      result.selected_move = you_move;
    }

    alpha = std::max(alpha, result.score);
    if (alpha >= beta) {
      ++stats.branches_pruned;
      break;
    }
  }

  return result;
}

int score_best_you_move(const GameState& state, int depth, int alpha, int beta, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  return search_best_you_move(state, depth, alpha, beta, deadline, params, stats).score;
}

BestMoveResult search_depth(const GameState& state, int depth, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  return search_best_you_move(state, depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), deadline, params, stats);
}

bool search_and_record_depth(const GameState& state, int depth, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats, Move& selected_move) {
  int nodes_before_depth = stats.nodes_evaluated;
  int prunes_before_depth = stats.branches_pruned;
  std::chrono::steady_clock::time_point depth_start = std::chrono::steady_clock::now();

  BestMoveResult result = search_depth(state, depth, deadline, params, stats);
  if (search_deadline_expired(deadline)) {
    return false;
  }
  
  stats.completed_depth = depth;
  stats.depth_stats.push_back({
    depth,
    result.selected_move,
    stats.nodes_evaluated - nodes_before_depth,
    stats.branches_pruned - prunes_before_depth,
    static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - depth_start).count()),
  });

  selected_move = result.selected_move;
  return true;
}

Move search_with_iterative_deepening(const GameState& state, std::chrono::steady_clock::time_point deadline, const EvaluationParams& params, MinimaxStats& stats) {
  Move selected_move = Move::Up;

  for (int depth = 1; depth <= MAX_ITERATIVE_DEEPENING_DEPTH; ++depth) {
    if (!search_and_record_depth(state, depth, deadline, params, stats, selected_move)) {
      break;
    }
  }

  return selected_move;
}

}

Move choose_minimax_move(const GameState& state, std::chrono::milliseconds latency_budget, const EvaluationParams& params, MinimaxStats& stats) {
  stats = MinimaxStats{};
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point deadline = start + latency_budget;

  Move selected_move = search_with_iterative_deepening(state, deadline, params, stats);

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  stats.elapsed_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  stats.timed_out = search_deadline_expired(deadline);

  return selected_move;
}

Move choose_minimax_move(const GameState& state, std::chrono::milliseconds latency_budget, MinimaxStats& stats) {
  return choose_minimax_move(state, latency_budget, EvaluationParams{}, stats);
}
