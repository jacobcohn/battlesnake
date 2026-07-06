#include "engine/hash/zobrist.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "engine/core/constants.hpp"

namespace engine::hash {

uint64_t compute_snake_body_hash(const Snake& snake, std::size_t snake_index) noexcept {
  assert(snake_index < MAX_SNAKES);

  if (!snake.alive || snake.length <= 1) {
    return 0;
  }

  assert(snake.tail >= 0 && snake.tail < BOARD_SQUARES);
  assert(snake.head >= 0 && snake.head < BOARD_SQUARES);

  uint64_t hash = 0;
  Square current = snake.tail;

  for (int visited = 0; current != snake.head && visited < BOARD_SQUARES; ++visited) {
    const Square next = snake.next[static_cast<std::size_t>(current)];
    assert(next >= 0 && next < BOARD_SQUARES);

    hash ^= edge_hash(snake_index, current, next);
    current = next;
  }

  assert(current == snake.head);
  return hash;
}

uint64_t compute_space_hash(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);

  uint64_t hash = 0;
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    hash ^= compute_snake_body_hash(position.snakes[index], index);
  }

  return hash;
}

uint64_t compute_exact_hash(const Position& position) noexcept {
  assert(position.snake_count <= MAX_SNAKES);
  assert(position.you < MAX_SNAKES);

  uint64_t hash = compute_space_hash(position);

  for (int square = 0; square < BOARD_SQUARES; ++square) {
    const Square square_index = static_cast<Square>(square);
    if (position.food.test(square_index)) {
      hash ^= food_hash(square_index);
    }
  }

  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    const Snake& snake = position.snakes[index];
    if (snake.alive) {
      hash ^= health_hash(index, snake.health);
    }
  }

  hash ^= turn_hash(position.turn);
  hash ^= you_hash(static_cast<std::size_t>(position.you));

  return hash;
}

void initialize_hashes(Position& position) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    position.snakes[index].body_hash = compute_snake_body_hash(position.snakes[index], index);
  }

  position.space_hash = compute_space_hash(position);
  position.exact_hash = compute_exact_hash(position);
}

}
