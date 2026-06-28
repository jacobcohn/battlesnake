#include "select_move.h"

#include "move.h"

std::string select_move(const GameState& state, int search_depth) {
  (void)search_depth;

  if (state.you.body.empty()) {
    return "up";
  }

  const Coordinate& head = state.you.body.front();

  if (apply_move(head, Move::Up).y < state.board_height) {
    return std::string(move_to_string(Move::Up));
  }

  if (apply_move(head, Move::Right).x < state.board_width) {
    return std::string(move_to_string(Move::Right));
  }

  if (apply_move(head, Move::Down).y >= 0) {
    return std::string(move_to_string(Move::Down));
  }

  return std::string(move_to_string(Move::Left));
}
