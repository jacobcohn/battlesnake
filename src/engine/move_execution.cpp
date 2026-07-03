#include "engine/move_execution.hpp"

#include <cstddef>

namespace {

void reset_undo(Undo& undo) noexcept {
  undo.old_heads.fill(INVALID_SQUARE);
  undo.old_tails.fill(INVALID_SQUARE);
  undo.new_heads.fill(INVALID_SQUARE);
  undo.old_health.fill(0);
  undo.ate_food.fill(false);
  undo.died.fill(false);
}

void prepare_undo(const Position& position, Undo& undo) noexcept {
  reset_undo(undo);

  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Snake& snake = position.snakes[index];

    if (!snake.alive || snake.length == 0) {
      continue;
    }

    undo.old_heads[index] = snake.head;
    undo.old_tails[index] = snake.tail;
    undo.old_health[index] = snake.health;
  }
}

void compute_move_targets(const Position& position, const JointMove& joint, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE) {
      undo.new_heads[index] = move_target(position.snakes[index].head, joint.moves[index]);
      undo.ate_food[index] = undo.new_heads[index] != INVALID_SQUARE && position.food.test(undo.new_heads[index]);
    }
  }
}

bool has_new_head_on_square(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] == square) {
      return true;
    }
  }

  return false;
}

void remove_tail(Position& position, Snake& snake, bool clear_occupied) noexcept {
  const Square old_tail = snake.tail;
  snake.tail = snake.next[static_cast<std::size_t>(old_tail)];
  snake.next[static_cast<std::size_t>(old_tail)] = INVALID_SQUARE;
  if (clear_occupied) {
    position.occupied.clear(old_tail);
  }
}

void move_heads(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] == INVALID_SQUARE) {
      continue;
    }

    const Square new_head = undo.new_heads[index];
    if (new_head == INVALID_SQUARE) {
      undo.died[index] = true;
      continue;
    }

    snake.next[static_cast<std::size_t>(snake.head)] = new_head;
    snake.head = new_head;
    position.occupied.set(new_head);
  }
}

void remove_non_growing_tails(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index]) {
      const bool clear_occupied = !has_new_head_on_square(position, undo, snake.tail);
      remove_tail(position, snake, clear_occupied);
    }
  }
}

void reduce_health(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] == INVALID_SQUARE || undo.new_heads[index] == INVALID_SQUARE) {
      continue;
    }

    const int health_after_move = static_cast<int>(snake.health) - 1;
    snake.health = static_cast<uint8_t>(health_after_move > 0 ? health_after_move : 0);
  }
}

void apply_food(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] == INVALID_SQUARE || undo.new_heads[index] == INVALID_SQUARE) {
      continue;
    }

    if (undo.ate_food[index]) {
      snake.health = 100;
      ++snake.length;
      position.food.clear(undo.new_heads[index]);
    }
  }
}

bool is_vacated_tail(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index] &&
        undo.old_tails[index] == square) {
      return true;
    }
  }

  return false;
}

bool square_was_occupied_at_start(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] == INVALID_SQUARE) {
      continue;
    }

    const Square old_tail = undo.old_tails[index];
    const Square old_head = undo.old_heads[index];

    if (square == old_tail) {
      return true;
    }

    if (old_tail == old_head) {
      continue;
    }

    const Snake& snake = position.snakes[index];
    Square current = undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index] ? snake.tail : old_tail;
    for (int visited = 0; current != INVALID_SQUARE && visited < BOARD_SQUARES; ++visited) {
      if (current == square) {
        return true;
      }
      if (current == old_head) {
        break;
      }
      current = snake.next[static_cast<std::size_t>(current)];
    }
  }

  return false;
}

bool hits_body_after_tail_updates(const Position& position, const Undo& undo, Square square) noexcept {
  return square_was_occupied_at_start(position, undo, square) && !is_vacated_tail(position, undo, square);
}

void mark_health_deaths(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && position.snakes[index].health == 0) {
      undo.died[index] = true;
    }
  }
}

void mark_body_collisions(const Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Square head = undo.new_heads[index];

    if (undo.old_heads[index] != INVALID_SQUARE && head != INVALID_SQUARE && hits_body_after_tail_updates(position, undo, head)) {
      undo.died[index] = true;
    }
  }
}

void mark_head_to_head_deaths(const Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t a = static_cast<std::size_t>(i);
    if (undo.old_heads[a] == INVALID_SQUARE || undo.new_heads[a] == INVALID_SQUARE) {
      continue;
    }

    for (int j = i + 1; j < position.snake_count; ++j) {
      const std::size_t b = static_cast<std::size_t>(j);
      if (undo.old_heads[b] == INVALID_SQUARE || undo.new_heads[b] == INVALID_SQUARE || undo.new_heads[a] != undo.new_heads[b]) {
        continue;
      }

      const uint8_t length_a = position.snakes[a].length;
      const uint8_t length_b = position.snakes[b].length;
      if (length_a == length_b) {
        undo.died[a] = true;
        undo.died[b] = true;
      } else if (length_a < length_b) {
        undo.died[a] = true;
      } else {
        undo.died[b] = true;
      }
    }
  }
}

bool square_is_in_surviving_snake(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] == INVALID_SQUARE || undo.died[index]) {
      continue;
    }

    const Snake& snake = position.snakes[index];
    Square current = snake.tail;
    for (int visited = 0; current != INVALID_SQUARE && visited < snake.length; ++visited) {
      if (current == square) {
        return true;
      }
      current = snake.next[static_cast<std::size_t>(current)];
    }
  }

  return false;
}

void clear_snake_from_occupied(Position& position, const Undo& undo, const Snake& snake) noexcept {
  Square current = snake.tail;
  for (int visited = 0; current != INVALID_SQUARE && visited < snake.length; ++visited) {
    const Square next = snake.next[static_cast<std::size_t>(current)];
    if (!square_is_in_surviving_snake(position, undo, current)) {
      position.occupied.clear(current);
    }
    current = next;
  }
}

void remove_dead_snakes(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] != INVALID_SQUARE && undo.died[index]) {
      clear_snake_from_occupied(position, undo, snake);
      snake.alive = false;
    }
  }
}

void set_body_in_occupied(Position& position, const Snake& snake) noexcept {
  Square current = snake.tail;
  for (int visited = 0; current != INVALID_SQUARE && visited < snake.length; ++visited) {
    position.occupied.set(current);
    current = snake.next[static_cast<std::size_t>(current)];
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

}

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
