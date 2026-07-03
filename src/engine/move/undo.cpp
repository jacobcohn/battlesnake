#include "internal.hpp"

#include <cstddef>

namespace {

void set_body_in_occupied(Position& position, const Snake& snake) noexcept {
  Square current = snake.tail;
  for (int visited = 0; current != INVALID_SQUARE && visited < snake.length; ++visited) {
    position.occupied.set(current);
    current = snake.next[static_cast<std::size_t>(current)];
  }
}

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
    }

    snake.next[static_cast<std::size_t>(undo.old_heads[index])] = INVALID_SQUARE;
    snake.head = undo.old_heads[index];
  }
}

}

void restore_survivor_occupied(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] == INVALID_SQUARE || undo.new_heads[index] == INVALID_SQUARE || undo.died[index]) {
      continue;
    }

    position.occupied.clear(undo.new_heads[index]);
    if (!undo.ate_food[index]) {
      position.occupied.set(undo.old_tails[index]);
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

void restore_dead_occupied(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && undo.died[index]) {
      set_body_in_occupied(position, position.snakes[index]);
    }
  }
}
