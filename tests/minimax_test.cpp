#include "minimax.h"

#include <cassert>

#include "move.h"
#include "test_helpers.h"

namespace {

void chooses_only_surviving_move() {
  GameState state = make_state(
    make_snake("you", {{0, 0}, {1, 0}, {2, 0}}),
    make_snake("opponent", {{4, 4}, {4, 3}, {4, 2}}),
    {},
    5,
    5);

  Move move = choose_minimax_move(state, 1);

  assert(move == Move::Up);
}

void chooses_food_when_safe() {
  GameState state = make_state(
    make_snake("you", {{2, 2}, {2, 1}, {2, 0}}),
    make_snake("opponent", {{6, 6}, {6, 5}, {6, 4}}),
    {{2, 3}},
    7,
    7);

  Move move = choose_minimax_move(state, 1);

  assert(move == Move::Up);
}

}

int main() {
  chooses_only_surviving_move();
  chooses_food_when_safe();
}
