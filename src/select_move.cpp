#include "select_move.h"

std::string select_move(int board_width, int board_height, int head_x, int head_y) {
  if (head_y + 1 < board_height) {
    return "up";
  }

  if (head_x + 1 < board_width) {
    return "right";
  }

  if (head_y - 1 >= 0) {
    return "down";
  }

  return "left";
}
