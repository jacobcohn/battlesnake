#include <cassert>

#include "evaluate.h"
#include "test_helpers.h"

namespace {
GameState make_default_state() {
  return make_state(
      make_snake("you", {{1, 1}, {1, 0}, {0, 0}}, 50),
      make_snake("opponent", {{5, 5}, {5, 4}, {5, 3}}, 50));
}
}

int main() {
  GameState win_state = make_default_state();
  win_state.opponent.alive = false;
  assert(evaluate_state(win_state) == 1'000'000);

  GameState loss_state = make_default_state();
  loss_state.you.alive = false;
  assert(evaluate_state(loss_state) == -1'000'000);

  GameState draw_state = make_default_state();
  draw_state.you.alive = false;
  draw_state.opponent.alive = false;
  assert(evaluate_state(draw_state) == -900'000);

  GameState higher_health_state = make_default_state();
  higher_health_state.you.health = 80;
  higher_health_state.opponent.health = 40;

  GameState lower_health_state = make_default_state();
  lower_health_state.you.health = 40;
  lower_health_state.opponent.health = 80;
  assert(evaluate_state(higher_health_state) > evaluate_state(lower_health_state));

  GameState longer_state = make_default_state();
  longer_state.you.body.push_back({0, 1});

  GameState shorter_state = make_default_state();
  shorter_state.opponent.body.push_back({4, 3});
  assert(evaluate_state(longer_state) > evaluate_state(shorter_state));

  GameState opponent_advantage_state = make_default_state();
  opponent_advantage_state.opponent.health = 90;
  opponent_advantage_state.opponent.body.push_back({4, 3});
  assert(evaluate_state(opponent_advantage_state) < evaluate_state(make_default_state()));

  return 0;
}
