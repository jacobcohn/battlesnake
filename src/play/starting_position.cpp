#include "play/starting_position.h"

GameState make_corner_start_state(int width, int height) {
  GameState state;
  state.board_width = width;
  state.board_height = height;

  state.you.id = "snake_a";
  state.you.health = 100;
  state.you.alive = true;
  state.you.body = {
    {0, 0},
    {1, 0},
    {1, 1},
  };

  state.opponent.id = "snake_b";
  state.opponent.health = 100;
  state.opponent.alive = true;
  state.opponent.body = {
    {width - 1, height - 1},
    {width - 2, height - 1},
    {width - 2, height - 2},
  };

  state.food = {
    {width / 2, height / 2},
  };

  return state;
}
