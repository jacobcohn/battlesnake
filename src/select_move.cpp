#include "select_move.h"

std::string select_move(const GameState& state, int search_depth) {
  (void)search_depth;

  if (state.you.body.empty()) {
    return "up";
  }

  const Coordinate& head = state.you.body.front();

  if (head.y + 1 < state.board_height) {
    return "up";
  }

  if (head.x + 1 < state.board_width) {
    return "right";
  }

  if (head.y - 1 >= 0) {
    return "down";
  }

  return "left";
}
