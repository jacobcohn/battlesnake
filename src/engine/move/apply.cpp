#include "internal.hpp"

#include <cstddef>

#include "engine/hash/zobrist.hpp"

namespace {

bool has_new_head_on_square(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] == square) {
      return true;
    }
  }

  return false;
}

void remove_tail(Position& position, Snake& snake, std::size_t snake_index, bool clear_position_occupied) noexcept {
  const Square old_tail = snake.tail;
  const Square new_tail = snake.next[static_cast<std::size_t>(old_tail)];
  snake.body_hash ^= engine::hash::edge_hash(snake_index, old_tail, new_tail);
  snake.tail = new_tail;
  snake.next[static_cast<std::size_t>(old_tail)] = INVALID_SQUARE;
  snake.occupied.clear(old_tail);
  if (clear_position_occupied) {
    position.occupied.clear(old_tail);
  }
}

}

void prepare_undo(const Position& position, Undo& undo) noexcept {
  undo.old_exact_hash = position.exact_hash;
  undo.old_space_hash = position.space_hash;

  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Snake& snake = position.snakes[index];

    if (!snake.alive || snake.length == 0) {
      continue;
    }

    undo.old_heads[index] = snake.head;
    undo.old_tails[index] = snake.tail;
    undo.old_health[index] = snake.health;
    undo.old_stacked_tail_segments[index] = snake.stacked_tail_segments;
    undo.old_body_hashes[index] = snake.body_hash;
  }
}

void compute_move_targets(const Position& position, const JointMove& joint, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE) {
      undo.new_heads[index] = move_target(position.snakes[index].head, joint.moves[index]);
      undo.ate_food[index] = undo.new_heads[index] != INVALID_SQUARE && position.food.test(undo.new_heads[index]);
      undo.new_head_was_own_body[index] = undo.new_heads[index] != INVALID_SQUARE &&
                                           position.snakes[index].occupied.test(undo.new_heads[index]);
    }
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

    snake.body_hash ^= engine::hash::edge_hash(index, snake.head, new_head);
    snake.next[static_cast<std::size_t>(snake.head)] = new_head;
    snake.head = new_head;
    snake.occupied.set(new_head);
    position.occupied.set(new_head);
  }
}

void remove_non_growing_tails(Position& position, Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] != INVALID_SQUARE && !undo.ate_food[index]) {
      if (snake.stacked_tail_segments > 0) {
        --snake.stacked_tail_segments;
      } else {
        const bool clear_occupied = !has_new_head_on_square(position, undo, snake.tail);
        remove_tail(position, snake, index, clear_occupied);
        undo.removed_tail[index] = true;
      }
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
