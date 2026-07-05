#include <array>
#include <cstddef>
#include <initializer_list>

#include <doctest/doctest.h>

#include "engine/core/position.hpp"
#include "engine/core/square.hpp"
#include "engine/evaluation/evaluation.hpp"
#include "engine/evaluation/territory/territory.hpp"

namespace {

void set_snake(Snake& snake, std::initializer_list<Square> head_to_tail, bool alive = true) {
  snake.reset();

  std::array<Square, BOARD_SQUARES> squares{};
  std::size_t count = 0;
  for (Square square : head_to_tail) {
    squares[count] = square;
    ++count;
  }

  REQUIRE(count > 0);
  snake.head = squares[0];
  snake.tail = squares[count - 1];
  snake.length = static_cast<uint8_t>(count);
  snake.health = 100;
  snake.alive = alive;

  for (std::size_t i = 0; i < count; ++i) {
    snake.occupied.set(squares[i]);
  }

  for (std::size_t i = count - 1; i > 0; --i) {
    snake.next[static_cast<std::size_t>(squares[i])] = squares[i - 1];
  }
}

Position two_snake_position(bool you_alive = true, bool opponent_alive = true) {
  Position position;
  position.reset();
  position.snake_count = 2;
  position.you = 0;
  set_snake(position.snakes[0], {idx(2, 5), idx(2, 4)}, you_alive);
  set_snake(position.snakes[1], {idx(8, 5), idx(8, 4)}, opponent_alive);
  position.build_occupied();
  return position;
}

int expected_territory_score(const Position& position) {
  const engine::evaluation::TerritoryControl control = engine::evaluation::compute_territory_control(position);
  int score = control[static_cast<std::size_t>(position.you)];

  for (int i = 0; i < position.snake_count; ++i) {
    if (i != position.you) {
      score -= control[static_cast<std::size_t>(i)];
    }
  }

  return score;
}

}

TEST_CASE("evaluation returns loss when our snake is dead and an opponent remains alive") {
  const Position position = two_snake_position(false, true);

  CHECK(engine::evaluation::evaluate(position) == engine::evaluation::LOSS_SCORE);
}

TEST_CASE("evaluation returns win when our snake is the only alive snake") {
  const Position position = two_snake_position(true, false);

  CHECK(engine::evaluation::evaluate(position) == engine::evaluation::WIN_SCORE);
}

TEST_CASE("evaluation returns draw when all snakes are dead") {
  const Position position = two_snake_position(false, false);

  CHECK(engine::evaluation::evaluate(position) == engine::evaluation::DRAW_SCORE);
}

TEST_CASE("evaluation uses territory difference for non-terminal positions") {
  const Position position = two_snake_position(true, true);

  CHECK(engine::evaluation::evaluate(position) == expected_territory_score(position));
}

TEST_CASE("evaluation turn adjustment prefers faster wins and slower losses or draws") {
  Position fast_win = two_snake_position(true, false);
  Position slow_win = two_snake_position(true, false);
  Position fast_loss = two_snake_position(false, true);
  Position slow_loss = two_snake_position(false, true);
  Position fast_draw = two_snake_position(false, false);
  Position slow_draw = two_snake_position(false, false);
  Position non_terminal_position = two_snake_position(true, true);

  fast_win.turn = 1;
  slow_win.turn = 3;
  fast_loss.turn = 1;
  slow_loss.turn = 3;
  fast_draw.turn = 1;
  slow_draw.turn = 3;
  non_terminal_position.turn = 3;

  CHECK(engine::evaluation::evaluate(fast_win) > engine::evaluation::evaluate(slow_win));
  CHECK(engine::evaluation::evaluate(slow_loss) > engine::evaluation::evaluate(fast_loss));
  CHECK(engine::evaluation::evaluate(slow_draw) > engine::evaluation::evaluate(fast_draw));
  CHECK(engine::evaluation::evaluate(non_terminal_position) == expected_territory_score(non_terminal_position));
}
