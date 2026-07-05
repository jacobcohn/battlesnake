#include "engine/evaluation/evaluation.hpp"

#include <cstddef>

#include "engine/evaluation/territory/territory.hpp"

namespace engine::evaluation {

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

[[nodiscard]] bool any_alive_opponent(const Position& position) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    if (i != position.you && position.snakes[static_cast<std::size_t>(i)].alive) {
      return true;
    }
  }

  return false;
}

[[nodiscard]] bool any_alive_snake(const Position& position) noexcept {
  for (int i = 0; i < position.snake_count; ++i) {
    if (position.snakes[static_cast<std::size_t>(i)].alive) {
      return true;
    }
  }

  return false;
}

[[nodiscard]] int territory_score(const Position& position) noexcept {
  const TerritoryControl control = compute_territory_control(position);
  int score = control[static_cast<std::size_t>(position.you)];

  for (int i = 0; i < position.snake_count; ++i) {
    if (i != position.you) {
      score -= control[static_cast<std::size_t>(i)];
    }
  }

  return score;
}

[[nodiscard]] int adjusted_win_score(const Position& position) noexcept {
  return WIN_SCORE - position.turn;
}

[[nodiscard]] int adjusted_draw_score(const Position& position) noexcept {
  return DRAW_SCORE + position.turn;
}

[[nodiscard]] int adjusted_loss_score(const Position& position) noexcept {
  return LOSS_SCORE + position.turn;
}

}

int evaluate(const Position& position) noexcept {
  if (!any_alive_snake(position)) {
    return adjusted_draw_score(position);
  }

  if (!alive(position, position.you)) {
    return adjusted_loss_score(position);
  }

  if (!any_alive_opponent(position)) {
    return adjusted_win_score(position);
  }

  return territory_score(position);
}

}
