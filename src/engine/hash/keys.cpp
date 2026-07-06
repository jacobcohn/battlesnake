#include "engine/hash/zobrist.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "engine/core/constants.hpp"
#include "internal.hpp"

namespace engine::hash {

uint64_t edge_hash(std::size_t snake_index, Square from, Square to) noexcept {
  assert(snake_index < MAX_SNAKES);
  const int direction = internal::direction_index(from, to);
  assert(direction >= 0 && direction < internal::DIRECTION_COUNT);
  return internal::EDGE_KEYS[snake_index][static_cast<std::size_t>(from)][static_cast<std::size_t>(direction)];
}

uint64_t food_hash(Square square) noexcept {
  assert(square >= 0 && square < BOARD_SQUARES);
  return internal::FOOD_KEYS[static_cast<std::size_t>(square)];
}

uint64_t health_hash(std::size_t snake_index, uint8_t health) noexcept {
  assert(snake_index < MAX_SNAKES);
  assert(health <= internal::MAX_HEALTH);
  return internal::HEALTH_KEYS[snake_index][static_cast<std::size_t>(health)];
}

uint64_t turn_hash(uint16_t turn) noexcept {
  return internal::turn_key(turn);
}

uint64_t you_hash(std::size_t snake_index) noexcept {
  assert(snake_index < MAX_SNAKES);
  return internal::YOU_KEYS[snake_index];
}

}
