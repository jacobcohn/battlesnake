#include "engine/move_execution.hpp"

void unmake_move(Position& position, const Undo& undo) noexcept {
  position.snakes = undo.snakes;
  position.occupied = undo.occupied;
  position.food = undo.food;
  position.hazards = undo.hazards;
  position.snake_count = undo.snake_count;
  position.you = undo.you;
  position.turn = undo.turn;
  position.exact_hash = undo.exact_hash;
  position.space_hash = undo.space_hash;
}
