#include "internal.hpp"

#include <cstddef>

namespace {

void restore_snake(Position& position, const Undo& undo, std::size_t index) noexcept {
  Snake& snake = position.snakes[index];
  const bool was_valid = undo.new_heads[index] != INVALID_SQUARE;
  const bool did_eat = undo.ate_food[index];

  if (undo.died[index]) {
    snake.alive = true;
  }

  snake.health = undo.old_health[index];

  if (was_valid) {
    if (did_eat) {
      --snake.length;
      position.food.set(undo.new_heads[index]);
    } else {
      snake.next[static_cast<std::size_t>(undo.old_tails[index])] = snake.tail;
      snake.tail = undo.old_tails[index];
      snake.occupied.set(undo.old_tails[index]);
    }

    if (!undo.new_head_was_own_body[index]) {
      snake.occupied.clear(undo.new_heads[index]);
    }
    snake.next[static_cast<std::size_t>(undo.old_heads[index])] = INVALID_SQUARE;
    snake.head = undo.old_heads[index];
  }
}

}

void restore_snakes(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE) {
      restore_snake(position, undo, index);
    }
  }
}
