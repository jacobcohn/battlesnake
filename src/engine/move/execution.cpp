#include "internal.hpp"

Undo make_move(Position& position, const JointMove& joint) noexcept {
  Undo undo;
  prepare_undo(position, undo);
  compute_move_targets(position, joint, undo);

  move_heads(position, undo);
  remove_non_growing_tails(position, undo);
  reduce_health(position, undo);
  apply_food(position, undo);
  mark_health_deaths(position, undo);
  mark_body_collisions(position, undo);
  mark_head_to_head_deaths(position, undo);
  remove_dead_snakes(position, undo);

  ++position.turn;
  return undo;
}

void unmake_move(Position& position, const Undo& undo) noexcept {
  restore_survivor_occupied(position, undo);
  restore_snakes(position, undo);
  restore_dead_occupied(position, undo);
  --position.turn;
}
