#include "evaluate.h"

#include "flood_fill.h"

namespace {
constexpr int WIN_SCORE = 1'000'000;
constexpr int LOSS_SCORE = -1'000'000;
constexpr int DRAW_SCORE = -900'000;
constexpr int HEALTH_WEIGHT = 1;
constexpr int LENGTH_WEIGHT = 25;
constexpr int REACHABLE_SPACE_WEIGHT = 8;
}

int evaluate_state(const GameState& state) {
  if (state.you.alive && !state.opponent.alive) {
    return WIN_SCORE;
  }

  if (!state.you.alive && state.opponent.alive) {
    return LOSS_SCORE;
  }

  if (!state.you.alive && !state.opponent.alive) {
    return DRAW_SCORE;
  }

  int health_difference = state.you.health - state.opponent.health;
  int length_difference = static_cast<int>(state.you.body.size()) - static_cast<int>(state.opponent.body.size());
  int reachable_space_difference = count_reachable_space_from_head(state, state.you) - count_reachable_space_from_head(state, state.opponent);

  int health_score = HEALTH_WEIGHT * health_difference;
  int length_score = LENGTH_WEIGHT * length_difference;
  int reachable_space_score = REACHABLE_SPACE_WEIGHT * reachable_space_difference;

  return health_score + length_score + reachable_space_score;
}
