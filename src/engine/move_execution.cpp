#include "engine/move_execution.hpp"

#include <cstddef>

namespace {

void reset_move_facts(Undo& undo) noexcept {
  undo.alive_at_start.fill(false);
  undo.old_tails.fill(INVALID_SQUARE);
  undo.new_heads.fill(INVALID_SQUARE);
  undo.ate_food.fill(false);
  undo.dies.fill(false);
}

void prepare_undo(const Position& position, Undo& undo) noexcept {
  undo.snakes = position.snakes;
  undo.occupied = position.occupied;
  undo.food = position.food;
  undo.snake_count = position.snake_count;
  undo.you = position.you;
  undo.turn = position.turn;
  undo.exact_hash = position.exact_hash;
  undo.space_hash = position.space_hash;
  reset_move_facts(undo);
}

void compute_move_targets(const Position& position, const JointMove& joint, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Snake& snake = position.snakes[index];

    if (!snake.alive || snake.length == 0) {
      continue;
    }

    undo.alive_at_start[index] = true;
    undo.old_tails[index] = snake.tail;
    undo.new_heads[index] = move_target(snake.head, joint.moves[index]);

    if (undo.new_heads[index] != INVALID_SQUARE) {
      undo.ate_food[index] = position.food.test(undo.new_heads[index]);
    }
  }
}

bool has_new_head_on_square(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.alive_at_start[index] && undo.new_heads[index] == square) {
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

    if (!undo.alive_at_start[index]) {
      continue;
    }

    const Square new_head = undo.new_heads[index];
    if (new_head == INVALID_SQUARE) {
      undo.dies[index] = true;
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

    if (undo.alive_at_start[index] && undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index]) {
      const bool clear_occupied = !has_new_head_on_square(position, undo, snake.tail);
      remove_tail(position, snake, clear_occupied);
    }
  }
}

void apply_food(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (!undo.alive_at_start[index] || undo.new_heads[index] == INVALID_SQUARE) {
      continue;
    }

    if (undo.ate_food[index]) {
      snake.health = 100;
      ++snake.length;
      position.food.clear(undo.new_heads[index]);
    }
  }
}

void reduce_health(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (!undo.alive_at_start[index] || undo.new_heads[index] == INVALID_SQUARE) {
      continue;
    }

    const int health_after_move = static_cast<int>(snake.health) - 1;
    snake.health = static_cast<uint8_t>(health_after_move > 0 ? health_after_move : 0);
  }
}

bool is_vacated_tail(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.alive_at_start[index] && undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index] &&
        undo.old_tails[index] == square) {
      return true;
    }
  }

  return false;
}

bool hits_body_after_tail_updates(const Position& position, const Undo& undo, Square square) noexcept {
  return undo.occupied.test(square) && !is_vacated_tail(position, undo, square);
}

void mark_health_deaths(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.alive_at_start[index] && position.snakes[index].health == 0) {
      undo.dies[index] = true;
    }
  }
}

void mark_body_collisions(const Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Square head = undo.new_heads[index];

    if (undo.alive_at_start[index] && head != INVALID_SQUARE && hits_body_after_tail_updates(position, undo, head)) {
      undo.dies[index] = true;
    }
  }
}

void mark_head_to_head_deaths(const Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t a = static_cast<std::size_t>(i);
    if (!undo.alive_at_start[a] || undo.new_heads[a] == INVALID_SQUARE) {
      continue;
    }

    for (int j = i + 1; j < position.snake_count; ++j) {
      const std::size_t b = static_cast<std::size_t>(j);
      if (!undo.alive_at_start[b] || undo.new_heads[b] == INVALID_SQUARE ||
          undo.new_heads[a] != undo.new_heads[b]) {
        continue;
      }

      const uint8_t length_a = position.snakes[a].length;
      const uint8_t length_b = position.snakes[b].length;
      if (length_a == length_b) {
        undo.dies[a] = true;
        undo.dies[b] = true;
      } else if (length_a < length_b) {
        undo.dies[a] = true;
      } else {
        undo.dies[b] = true;
      }
    }
  }
}

bool square_is_in_surviving_snake(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (!undo.alive_at_start[index] || undo.dies[index]) {
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

    if (undo.alive_at_start[index] && undo.dies[index]) {
      clear_snake_from_occupied(position, undo, snake);
      snake.alive = false;
    }
  }
}

}

void make_move(Position& position, const JointMove& joint, Undo& undo) noexcept {
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
}

void unmake_move(Position& position, const Undo& undo) noexcept {
  position.snakes = undo.snakes;
  position.occupied = undo.occupied;
  position.food = undo.food;
  position.snake_count = undo.snake_count;
  position.you = undo.you;
  position.turn = undo.turn;
  position.exact_hash = undo.exact_hash;
  position.space_hash = undo.space_hash;
}
