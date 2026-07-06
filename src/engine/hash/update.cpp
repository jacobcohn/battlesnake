#include "engine/hash/zobrist.hpp"

#include <cstddef>
#include <cstdint>

#include "engine/core/constants.hpp"
#include "engine/move/execution.hpp"

namespace engine::hash {
namespace {

bool ate_food_already_processed(const Undo& undo, std::size_t index) noexcept {
  for (std::size_t previous = 0; previous < index; ++previous) {
    if (undo.ate_food[previous] && undo.new_heads[previous] == undo.new_heads[index]) {
      return true;
    }
  }

  return false;
}

}

void update_after_move(Position& position, const Undo& undo) noexcept {
  uint64_t space = undo.old_space_hash;

  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] == INVALID_SQUARE) {
      continue;
    }

    space ^= undo.old_body_hashes[index];
    if (position.snakes[index].alive) {
      space ^= position.snakes[index].body_hash;
    }
  }

  uint64_t exact = undo.old_exact_hash;
  exact ^= undo.old_space_hash;
  exact ^= space;

  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] == INVALID_SQUARE) {
      continue;
    }

    exact ^= health_hash(index, undo.old_health[index]);
    if (position.snakes[index].alive) {
      exact ^= health_hash(index, position.snakes[index].health);
    }

    if (undo.ate_food[index] && !ate_food_already_processed(undo, index)) {
      exact ^= food_hash(undo.new_heads[index]);
    }
  }

  exact ^= turn_hash(static_cast<uint16_t>(position.turn - 1));
  exact ^= turn_hash(position.turn);

  position.space_hash = space;
  position.exact_hash = exact;
}

void restore_after_unmake(Position& position, const Undo& undo) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    if (undo.old_heads[index] != INVALID_SQUARE) {
      position.snakes[index].body_hash = undo.old_body_hashes[index];
    }
  }

  position.exact_hash = undo.old_exact_hash;
  position.space_hash = undo.old_space_hash;
}

}
