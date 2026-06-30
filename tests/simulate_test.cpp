#include <cassert>

#include "play/simulate.h"
#include "test_helpers.h"

void test_wall_death() {
  GameState state = make_state(make_snake("you", {{0, 0}}), make_snake("opponent", {{5, 5}}));

  GameState result = simulate_turn(state, Move::Left, Move::Up);

  assert(!result.you.alive);
  assert(result.opponent.alive);
}

void test_starvation_death() {
  GameState state = make_state(make_snake("you", {{5, 5}}, 1), make_snake("opponent", {{8, 8}}));

  GameState result = simulate_turn(state, Move::Up, Move::Up);

  assert(!result.you.alive);
  assert(result.opponent.alive);
}

void test_food_growth_and_health_reset() {
  GameState state = make_state(make_snake("you", {{5, 5}, {5, 4}}, 20), make_snake("opponent", {{9, 9}}), {{5, 6}});

  GameState result = simulate_turn(state, Move::Up, Move::Left);

  assert(result.you.alive);
  assert(result.you.health == 100);
  assert(result.you.body.size() == 3);
  assert(result.food.empty());
}

void test_normal_move_removes_tail() {
  GameState state = make_state(make_snake("you", {{5, 5}, {5, 4}, {5, 3}}), make_snake("opponent", {{9, 9}}));

  GameState result = simulate_turn(state, Move::Up, Move::Left);

  assert(result.you.alive);
  assert(result.you.body.size() == 3);
  assert(result.you.body.front().x == 5);
  assert(result.you.body.front().y == 6);
  assert(result.you.body.back().x == 5);
  assert(result.you.body.back().y == 4);
}

void test_head_to_head_equal_length() {
  GameState state = make_state(make_snake("you", {{4, 5}, {4, 4}}), make_snake("opponent", {{6, 5}, {6, 4}}));

  GameState result = simulate_turn(state, Move::Right, Move::Left);

  assert(!result.you.alive);
  assert(!result.opponent.alive);
}

void test_head_to_head_longer_wins() {
  GameState state = make_state(make_snake("you", {{4, 5}, {4, 4}, {4, 3}}), make_snake("opponent", {{6, 5}, {6, 4}}));

  GameState result = simulate_turn(state, Move::Right, Move::Left);

  assert(result.you.alive);
  assert(!result.opponent.alive);
}

void test_moving_into_own_vacated_tail_is_safe() {
  GameState state = make_state(make_snake("you", {{5, 5}, {5, 4}, {4, 4}, {4, 5}}), make_snake("opponent", {{9, 9}}));

  GameState result = simulate_turn(state, Move::Left, Move::Left);

  assert(result.you.alive);
}

void test_moving_into_opponent_tail_when_opponent_eats_is_death() {
  GameState state = make_state(make_snake("you", {{4, 5}}), make_snake("opponent", {{8, 8}, {8, 7}, {5, 5}}), {{8, 9}});

  GameState result = simulate_turn(state, Move::Right, Move::Up);

  assert(!result.you.alive);
  assert(result.opponent.alive);
}

void test_body_collision_death() {
  GameState state = make_state(make_snake("you", {{4, 5}}), make_snake("opponent", {{8, 8}, {5, 5}, {5, 4}}));

  GameState result = simulate_turn(state, Move::Right, Move::Left);

  assert(!result.you.alive);
}

int main() {
  test_wall_death();
  test_starvation_death();
  test_food_growth_and_health_reset();
  test_normal_move_removes_tail();
  test_head_to_head_equal_length();
  test_head_to_head_longer_wins();
  test_moving_into_own_vacated_tail_is_safe();
  test_moving_into_opponent_tail_when_opponent_eats_is_death();
  test_body_collision_death();

  return 0;
}
