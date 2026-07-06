#include "internal.hpp"

#include <cstddef>

namespace {

bool is_vacated_tail(const Position& position, const Undo& undo, Square square) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE && undo.new_heads[index] != INVALID_SQUARE && undo.removed_tail[index] &&
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

}

void build_alive_occupied(Position& position) noexcept {
  position.build_occupied();
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

void remove_dead_snakes(Position& position, const Undo& undo) noexcept {
  bool removed_any = false;
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    Snake& snake = position.snakes[index];

    if (undo.old_heads[index] != INVALID_SQUARE && undo.died[index]) {
      snake.alive = false;
      removed_any = true;
    }
  }

  if (removed_any) {
    build_alive_occupied(position);
  }
}
