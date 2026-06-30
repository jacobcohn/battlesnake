#include "play/play.h"

#include <chrono>

#include "minimax.h"
#include "play/food.h"
#include "play/starting_position.h"
#include "simulate.h"

namespace {

constexpr std::chrono::milliseconds SELF_PLAY_MOVE_BUDGET{50};

GameState swap_players(const GameState& state) {
  GameState swapped = state;
  swapped.you = state.opponent;
  swapped.opponent = state.you;
  return swapped;
}

GameOutcome determine_outcome(const GameState& state) {
  if (state.you.alive && state.opponent.alive) {
    return GameOutcome::Playing;
  }

  if (state.you.alive && !state.opponent.alive) {
    return GameOutcome::SnakeAWin;
  }

  if (!state.you.alive && state.opponent.alive) {
    return GameOutcome::SnakeBWin;
  }

  return GameOutcome::Draw;
}

Game initialize_game(
  int width,
  int height,
  const EvaluationParams& snake_a,
  const EvaluationParams& snake_b) {
  GameState state = make_corner_start_state(width, height);

  Game game;
  game.snake_a = snake_a;
  game.snake_b = snake_b;
  game.states.push_back(state);
  game.outcome = determine_outcome(state);

  return game;
}

Move choose_snake_a_move(const GameState& state, const EvaluationParams& snake_a) {
  MinimaxStats stats;
  return choose_minimax_move(state, SELF_PLAY_MOVE_BUDGET, snake_a, stats);
}

Move choose_snake_b_move(const GameState& state, const EvaluationParams& snake_b) {
  GameState snake_b_view = swap_players(state);
  MinimaxStats stats;
  return choose_minimax_move(snake_b_view, SELF_PLAY_MOVE_BUDGET, snake_b, stats);
}

}

Game play_game(
  int width,
  int height,
  const EvaluationParams& snake_a,
  const EvaluationParams& snake_b,
  const FoodRules& food_rules) {

  Game game = initialize_game(width, height, snake_a, snake_b);
  GameState state = game.states.back();

  while (game.outcome == GameOutcome::Playing) {
    Move snake_a_move = choose_snake_a_move(state, snake_a);
    Move snake_b_move = choose_snake_b_move(state, snake_b);

    state = simulate_turn(state, snake_a_move, snake_b_move);

    if (state.you.alive || state.opponent.alive) {
      apply_food_rules(state, food_rules);
    }

    game.states.push_back(state);
    game.outcome = determine_outcome(state);
  }

  return game;
}
