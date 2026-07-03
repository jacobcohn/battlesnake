#include "engine/move_execution.hpp"

#include <cstddef>

namespace {

struct MoveState {
  std::array<bool, MAX_SNAKES> alive_at_start{};
  std::array<Square, MAX_SNAKES> old_tails{};
  std::array<Square, MAX_SNAKES> new_heads{};
  std::array<uint8_t, MAX_SNAKES> old_lengths{};
  std::array<bool, MAX_SNAKES> ate_food{};
  std::array<bool, MAX_SNAKES> dies{};

  constexpr MoveState() noexcept {
    old_tails.fill(INVALID_SQUARE);
    new_heads.fill(INVALID_SQUARE);
  }
};

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

void collect_move_facts(const Position& position, const JointMove& joint, MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Snake& snake = position.snakes[index];

    if (!snake.alive || snake.length == 0) {
      continue;
    }

    state.alive_at_start[index] = true;
    state.old_tails[index] = snake.tail;
    state.old_lengths[index] = snake.length;
    state.new_heads[index] = move_target(snake.head, joint.moves[index]);

    if (state.new_heads[index] != INVALID_SQUARE) {
      state.ate_food[index] = position.food.test(state.new_heads[index]);
    }
  }
}

bool is_vacating_tail(const Position& position, Square square, const MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (state.alive_at_start[index] && !state.ate_food[index] && state.old_tails[index] == square) {
      return true;
    }
  }

  return false;
}

bool hits_body(const Position& position, Square square, const MoveState& state) noexcept {
  return position.occupied.test(square) && !is_vacating_tail(position, square, state);
}

void remove_tail(Snake& snake) noexcept {
  const Square old_tail = snake.tail;
  snake.tail = snake.next[static_cast<std::size_t>(old_tail)];
  snake.next[static_cast<std::size_t>(old_tail)] = INVALID_SQUARE;
}

void apply_single_move(Position& position, Snake& snake, std::size_t index, MoveState& state) noexcept {
  if (state.new_heads[index] == INVALID_SQUARE) {
    state.dies[index] = true;
    return;
  }

  snake.next[static_cast<std::size_t>(snake.head)] = state.new_heads[index];
  snake.head = state.new_heads[index];

  if (state.ate_food[index]) {
    snake.health = 100;
    ++snake.length;
    position.food.clear(state.new_heads[index]);
    return;
  }

  const int health_after_move = static_cast<int>(snake.health) - 1;
  snake.health = static_cast<uint8_t>(health_after_move > 0 ? health_after_move : 0);
  remove_tail(snake);

  if (health_after_move <= 0) {
    state.dies[index] = true;
  }
}

void apply_moves(Position& position, MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (state.alive_at_start[index]) {
      apply_single_move(position, position.snakes[index], index, state);
    }
  }
}

void resolve_body_collisions(const Position& position, MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Square head = state.new_heads[index];

    if (state.alive_at_start[index] && head != INVALID_SQUARE && hits_body(position, head, state)) {
      state.dies[index] = true;
    }
  }
}

void resolve_head_to_head(const Position& position, MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t a = static_cast<std::size_t>(i);
    if (!state.alive_at_start[a] || state.new_heads[a] == INVALID_SQUARE) {
      continue;
    }

    for (int j = i + 1; j < position.snake_count; ++j) {
      const std::size_t b = static_cast<std::size_t>(j);
      if (!state.alive_at_start[b] || state.new_heads[b] == INVALID_SQUARE ||
          state.new_heads[a] != state.new_heads[b]) {
        continue;
      }

      if (state.old_lengths[a] == state.old_lengths[b]) {
        state.dies[a] = true;
        state.dies[b] = true;
      } else if (state.old_lengths[a] < state.old_lengths[b]) {
        state.dies[a] = true;
      } else {
        state.dies[b] = true;
      }
    }
  }
}

void remove_dead_snakes(Position& position, const MoveState& state) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (state.alive_at_start[index] && state.dies[index]) {
      position.snakes[index].reset();
    }
  }
}

}

void make_move(Position& position, const JointMove& joint, Undo& undo) noexcept {
  save_undo(position, undo);

  MoveState state;
  collect_move_facts(position, joint, state);

  apply_moves(position, state);
  resolve_body_collisions(position, state);
  resolve_head_to_head(position, state);
  remove_dead_snakes(position, state);

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
