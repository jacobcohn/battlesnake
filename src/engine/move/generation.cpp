#include "engine/move/generation.hpp"

#include <array>
#include <cassert>
#include <cstddef>

namespace engine::move {

namespace {

inline constexpr int ALIVE_MASK_COUNT = 1 << MAX_SNAKES;

inline constexpr std::array<Move, 4> ALL_MOVES{
    Move::Up,
    Move::Down,
    Move::Left,
    Move::Right,
};

struct JointMoveTable {
  std::array<std::array<JointMoveList, ALIVE_MASK_COUNT>, MAX_SNAKES + 1> lists{};
};

constexpr void append_joint_move(JointMoveList& list, const JointMove& joint) noexcept {
  if (list.count < MAX_JOINT_MOVES) {
    list.moves[static_cast<std::size_t>(list.count)] = joint;
    ++list.count;
  }
}

constexpr void generate_table_entry_recursive(
    int snake_count,
    int alive_mask,
    int snake_index,
    JointMove& current,
    JointMoveList& list) noexcept {
  if (snake_index >= snake_count) {
    append_joint_move(list, current);
    return;
  }

  const std::size_t index = static_cast<std::size_t>(snake_index);
  const bool alive = (alive_mask & (1 << snake_index)) != 0;
  if (!alive) {
    current.moves[index] = Move::Up;
    generate_table_entry_recursive(snake_count, alive_mask, snake_index + 1, current, list);
    return;
  }

  for (Move move : ALL_MOVES) {
    current.moves[index] = move;
    generate_table_entry_recursive(snake_count, alive_mask, snake_index + 1, current, list);
  }
}

[[nodiscard]] constexpr JointMoveList make_table_entry(int snake_count, int alive_mask) noexcept {
  JointMoveList list;
  JointMove current;
  current.moves.fill(Move::Up);

  generate_table_entry_recursive(snake_count, alive_mask, 0, current, list);
  return list;
}

[[nodiscard]] constexpr JointMoveTable make_joint_move_table() noexcept {
  JointMoveTable table;

  for (int snake_count = 0; snake_count <= MAX_SNAKES; ++snake_count) {
    const int used_mask = (1 << snake_count) - 1;
    for (int alive_mask = 0; alive_mask < ALIVE_MASK_COUNT; ++alive_mask) {
      table.lists[static_cast<std::size_t>(snake_count)][static_cast<std::size_t>(alive_mask)] =
          make_table_entry(snake_count, alive_mask & used_mask);
    }
  }

  return table;
}

inline constexpr JointMoveTable JOINT_MOVE_TABLE = make_joint_move_table();

[[nodiscard]] int alive_mask(const Position& position) noexcept {
  int mask = 0;

  for (int i = 0; i < position.snake_count; ++i) {
    if (position.snakes[static_cast<std::size_t>(i)].alive) {
      mask |= 1 << i;
    }
  }

  return mask;
}

}

const JointMoveList& generate_joint_moves(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  return JOINT_MOVE_TABLE.lists[static_cast<std::size_t>(position.snake_count)]
                               [static_cast<std::size_t>(alive_mask(position))];
}

}
