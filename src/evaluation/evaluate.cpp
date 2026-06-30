#include "evaluation/evaluate.h"

#include "evaluation/flood_fill.h"
#include "evaluation/territory.h"

namespace {

constexpr int WIN_SCORE = 1'000'000;
constexpr int LOSS_SCORE = -1'000'000;
constexpr int DRAW_SCORE = -900'000;

int evaluate_terminal_state(const GameState& state) {
  if (state.you.alive && !state.opponent.alive) {
    return WIN_SCORE;
  }

  if (!state.you.alive && state.opponent.alive) {
    return LOSS_SCORE;
  }

  if (!state.you.alive && !state.opponent.alive) {
    return DRAW_SCORE;
  }

  return 0;
}

int evaluate_health(const GameState& state, const EvaluationParams& params) {
  if (state.you.health < params.low_health_threshold) {
    return -(params.low_health_threshold - state.you.health) * params.low_health_penalty_weight;
  }

  return 0;
}

int evaluate_length_advantage(const GameState& state, const EvaluationParams& params) {
  if (state.you.body.size() > state.opponent.body.size()) {
    return params.length_advantage_score;
  }

  if (state.you.body.size() < state.opponent.body.size()) {
    return -params.length_advantage_score;
  }

  return 0;
}

int evaluate_reachable_space(const GameState& state, const EvaluationParams& params) {
  int you_reachable_space = count_reachable_space_from_head(state, state.you);
  int opponent_reachable_space = count_reachable_space_from_head(state, state.opponent);
  int reachable_space_difference = you_reachable_space - opponent_reachable_space;

  return params.reachable_space_weight * reachable_space_difference;
}

int evaluate_territory_control_score(const GameState& state, const EvaluationParams& params) {
  return params.territory_control_weight * evaluate_territory_control(state);
}

}

int evaluate_state(const GameState& state, const EvaluationParams& params) {
  if (!state.you.alive || !state.opponent.alive) {
    return evaluate_terminal_state(state);
  }

  int score = 0;
  score += evaluate_health(state, params);
  score += evaluate_length_advantage(state, params);
  score += evaluate_reachable_space(state, params);
  score += evaluate_territory_control_score(state, params);
  return score;
}

int evaluate_state(const GameState& state) {
  return evaluate_state(state, EvaluationParams{});
}
