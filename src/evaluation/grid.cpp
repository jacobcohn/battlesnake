#include "evaluation/grid.h"

bool is_out_of_bounds(Coordinate coordinate, int board_width, int board_height) {
  if (coordinate.x < 0) {
    return true;
  }

  if (coordinate.x >= board_width) {
    return true;
  }

  if (coordinate.y < 0) {
    return true;
  }

  if (coordinate.y >= board_height) {
    return true;
  }

  return false;
}

void mark_snake_body_as_blocked(Grid& blocked, const Snake& snake, int board_width, int board_height) {
  for (Coordinate coordinate : snake.body) {
    if (!is_out_of_bounds(coordinate, board_width, board_height)) {
      blocked[coordinate.y][coordinate.x] = true;
    }
  }
}

Grid build_blocked_grid(const GameState& state) {
  Grid blocked(state.board_height, std::vector<bool>(state.board_width, false));
  mark_snake_body_as_blocked(blocked, state.you, state.board_width, state.board_height);
  mark_snake_body_as_blocked(blocked, state.opponent, state.board_width, state.board_height);
  return blocked;
}
