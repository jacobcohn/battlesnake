#include "evaluation/evaluate.h"

#include "evaluation/flood_fill.h"
#include "evaluation/territory.h"

namespace {

constexpr int WIN_SCORE = 1'000'000;
constexpr int LOSS_SCORE = -1'000'000;
constexpr int DRAW_SCORE = -900'000;

constexpr int REACHABLE_SPACE_WEIGHT = 3;
constexpr int TERRITORY_CONTROL_WEIGHT = 12;
constexpr int LENGTH_ADVANTAGE_SCORE = 75;
constexpr int LOW_HEALTH_THRESHOLD = 25;
constexpr int LOW_HEALTH_PENALTY_WEIGHT = 15;

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

int evaluate_health(const GameState& state) {
  if (state.you.health < LOW_HEALTH_THRESHOLD) {
    return -(LOW_HEALTH_THRESHOLD - state.you.health) * LOW_HEALTH_PENALTY_WEIGHT;
  }

  return 0;
}

int evaluate_length_advantage(const GameState& state) {
  if (state.you.body.size() > state.opponent.body.size()) {
    return LENGTH_ADVANTAGE_SCORE;
  }

  if (state.you.body.size() < state.opponent.body.size()) {
    return -LENGTH_ADVANTAGE_SCORE;
  }

  return 0;
}

int evaluate_reachable_space(const GameState& state) {
  int you_reachable_space = count_reachable_space_from_head(state, state.you);
  int opponent_reachable_space = count_reachable_space_from_head(state, state.opponent);
  int reachable_space_difference = you_reachable_space - opponent_reachable_space;

  return REACHABLE_SPACE_WEIGHT * reachable_space_difference;
}

int evaluate_territory_control_score(const GameState& state) {
  return TERRITORY_CONTROL_WEIGHT * evaluate_territory_control(state);
}

}

int evaluate_state(const GameState& state) {
  if (!state.you.alive || !state.opponent.alive) {
    return evaluate_terminal_state(state);
  }

  int score = 0;
  score += evaluate_health(state);
  score += evaluate_length_advantage(state);
  score += evaluate_reachable_space(state);
  score += evaluate_territory_control_score(state);
  return score;
}
