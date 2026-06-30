#include "play/move.h"

Coordinate apply_move(Coordinate coordinate, Move move) {
  switch (move) {
    case Move::Up:
      coordinate.y += 1;
      break;
    case Move::Right:
      coordinate.x += 1;
      break;
    case Move::Down:
      coordinate.y -= 1;
      break;
    case Move::Left:
      coordinate.x -= 1;
      break;
  }

  return coordinate;
}

std::string_view move_to_string(Move move) {
  switch (move) {
    case Move::Up:
      return "up";
    case Move::Right:
      return "right";
    case Move::Down:
      return "down";
    case Move::Left:
      return "left";
  }

  return "up";
}
