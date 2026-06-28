#include "minimax.h"

#include <limits>
#include <random>
#include <vector>

#include "evaluate.h"
#include "simulate.h"

namespace {

int minimax_score(const GameState& state, int depth);

int score_move(const GameState& state, Move you_move, int depth) {
  int worst_score = std::numeric_limits<int>::max();

  for (Move opponent_move : ALL_MOVES) {
    GameState next_state = simulate_turn(state, you_move, opponent_move);
    int score = minimax_score(next_state, depth - 1);
    worst_score = std::min(worst_score, score);
  }

  return worst_score;
}

int minimax_score(const GameState& state, int depth) {
  if (depth == 0 || !state.you.alive || !state.opponent.alive) {
    return evaluate_state(state);
  }

  int best_score = std::numeric_limits<int>::min();

  for (Move you_move : ALL_MOVES) {
    best_score = std::max(best_score, score_move(state, you_move, depth));
  }

  return best_score;
}

std::vector<Move> best_moves_for_depth(const GameState& state, int depth) {
  std::vector<Move> best_moves;
  int best_score = std::numeric_limits<int>::min();

  for (Move you_move : ALL_MOVES) {
    int move_score = score_move(state, you_move, depth);

    if (move_score > best_score) {
      best_score = move_score;
      best_moves.clear();
      best_moves.push_back(you_move);
    } else if (move_score == best_score) {
      best_moves.push_back(you_move);
    }
  }

  return best_moves;
}

Move choose_random_move(const std::vector<Move>& moves) {
  static std::random_device random_device;
  static std::mt19937 generator(random_device());

  std::uniform_int_distribution<std::size_t> distribution(0, moves.size() - 1);
  return moves[distribution(generator)];
}

}

Move choose_minimax_move(const GameState& state, int depth) {
  std::vector<Move> best_moves = best_moves_for_depth(state, depth);

  if (best_moves.empty()) {
    return Move::Up;
  }

  return choose_random_move(best_moves);
}
