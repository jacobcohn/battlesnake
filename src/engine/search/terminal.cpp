#include "internal.hpp"

#include <cstddef>

namespace engine::search::internal {

namespace {

[[nodiscard]] bool alive(const Position& position, int snake_index) noexcept {
  if (snake_index < 0) {
    return false;
  }

  if (snake_index >= position.snake_count) {
    return false;
  }

  return position.snakes[static_cast<std::size_t>(snake_index)].alive;
}

[[nodiscard]] bool any_alive_snake(const Position& position) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    if (position.snakes[static_cast<std::size_t>(i)].alive) {
      return true;
    }
  }

  return false;
}

[[nodiscard]] bool any_alive_opponent(const Position& position) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    if (i != position.you && position.snakes[static_cast<std::size_t>(i)].alive) {
      return true;
    }
  }

  return false;
}

}

bool terminal(const Position& position) noexcept {
  if (!any_alive_snake(position)) {
    return true;
  }

  if (!alive(position, position.you)) {
    return true;
  }

  return !any_alive_opponent(position);
}

}
