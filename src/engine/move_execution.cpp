#include "engine/move_execution.hpp"

#include <cstddef>

namespace {

void save_undo(const Position& position, Undo& undo) noexcept {
  undo.snakes = position.snakes;
  undo.occupied = position.occupied;
  undo.food = position.food;
  undo.hazards = position.hazards;
  undo.snake_count = position.snake_count;
  undo.you = position.you;
  undo.turn = position.turn;
  undo.exact_hash = position.exact_hash;
  undo.space_hash = position.space_hash;
}

}

void make_move(Position& position, const JointMove& joint, Undo& undo) noexcept {
  save_undo(position, undo);

  for (int i = 0; i < position.snake_count; ++i) {
    Snake& snake = position.snakes[static_cast<std::size_t>(i)];

    if (!snake.alive || snake.length == 0) {
      continue;
    }

    const Square new_head = move_target(snake.head, joint.moves[static_cast<std::size_t>(i)]);
    if (new_head == INVALID_SQUARE) {
      snake.alive = false;
      continue;
    }

    const bool ate_food = position.food.test(new_head);
    const int health_after_move = static_cast<int>(snake.health) - 1;

    snake.next[static_cast<std::size_t>(snake.head)] = new_head;
    snake.head = new_head;

    if (ate_food) {
      snake.health = 100;
      ++snake.length;
      position.food.clear(new_head);
    } else {
      snake.health = static_cast<uint8_t>(health_after_move > 0 ? health_after_move : 0);

      const Square old_tail = snake.tail;
      snake.tail = snake.next[static_cast<std::size_t>(old_tail)];
      snake.next[static_cast<std::size_t>(old_tail)] = INVALID_SQUARE;

      if (health_after_move <= 0) {
        snake.alive = false;
      }
    }
  }

  ++position.turn;
  position.rebuild_occupied();
}

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
