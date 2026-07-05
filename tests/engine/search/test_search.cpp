#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>

#include <doctest/doctest.h>

#include "engine/core/square.hpp"
#include "engine/evaluation/evaluation.hpp"
#include "engine/search/search.hpp"

namespace {

void set_snake(Snake& snake, std::initializer_list<Square> head_to_tail, uint8_t health = 100) {
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
  snake.health = health;
  snake.alive = true;

  for (std::size_t i = 0; i < count; ++i) {
    snake.occupied.set(squares[i]);
  }

  for (std::size_t i = count - 1; i > 0; --i) {
    snake.next[static_cast<std::size_t>(squares[i])] = squares[i - 1];
  }
}

Position two_snake_position(
    std::initializer_list<Square> you_body,
    std::initializer_list<Square> opponent_body,
    uint8_t you_health = 100,
    uint8_t opponent_health = 100) {
  Position position;
  position.reset();
  position.snake_count = 2;
  position.you = 0;
  set_snake(position.snakes[0], you_body, you_health);
  set_snake(position.snakes[1], opponent_body, opponent_health);
  position.build_occupied();
  return position;
}

void check_position_equals(const Position& actual, const Position& expected) {
  CHECK(actual.occupied == expected.occupied);
  CHECK(actual.food == expected.food);
  CHECK(actual.snake_count == expected.snake_count);
  CHECK(actual.you == expected.you);
  CHECK(actual.turn == expected.turn);
  CHECK(actual.exact_hash == expected.exact_hash);
  CHECK(actual.space_hash == expected.space_hash);

  for (int i = 0; i < expected.snake_count; ++i) {
    const std::size_t index = static_cast<std::size_t>(i);
    CHECK(actual.snakes[index].head == expected.snakes[index].head);
    CHECK(actual.snakes[index].tail == expected.snakes[index].tail);
    CHECK(actual.snakes[index].length == expected.snakes[index].length);
    CHECK(actual.snakes[index].health == expected.snakes[index].health);
    CHECK(actual.snakes[index].alive == expected.snakes[index].alive);
    CHECK(actual.snakes[index].next == expected.snakes[index].next);
    CHECK(actual.snakes[index].occupied == expected.snakes[index].occupied);
  }
}

}

TEST_CASE("search depth zero returns static evaluation") {
  Position position = two_snake_position({idx(1, 1)}, {idx(9, 9)});

  engine::search::SearchLimits limits;
  limits.max_depth = 0;

  const engine::search::SearchResult result = engine::search::search(position, limits);

  CHECK(result.depth == 0);
  CHECK(result.completed);
  CHECK(result.score == engine::evaluation::evaluate(position));
}

TEST_CASE("search completes requested untimed depth") {
  Position position = two_snake_position({idx(1, 1)}, {idx(9, 9)});
  const Position before = position;

  engine::search::SearchLimits limits;
  limits.max_depth = 2;
  limits.time_limit_ms = 0;

  const engine::search::SearchResult result = engine::search::search(position, limits);

  CHECK(result.completed);
  CHECK(result.depth == 2);
  check_position_equals(position, before);
}

TEST_CASE("search avoids immediate off-board death when survival exists") {
  Position position = two_snake_position({idx(0, 0)}, {idx(9, 9)});
  const Position before = position;

  engine::search::SearchLimits limits;
  limits.max_depth = 1;

  const engine::search::SearchResult result = engine::search::search(position, limits);

  CHECK(result.completed);
  CHECK(result.depth == 1);
  CHECK(result.best_move.moves[static_cast<std::size_t>(position.you)] != Move::Left);
  CHECK(result.best_move.moves[static_cast<std::size_t>(position.you)] != Move::Down);
  CHECK(result.score > engine::evaluation::LOSS_SCORE);
  check_position_equals(position, before);
}

TEST_CASE("timed iterative deepening returns a restored position") {
  Position position = two_snake_position({idx(1, 1)}, {idx(9, 9)});
  const Position before = position;

  engine::search::SearchLimits limits;
  limits.max_depth = 8;
  limits.time_limit_ms = 1;

  const engine::search::SearchResult result = engine::search::search(position, limits);

  CHECK(result.depth >= 0);
  CHECK(result.depth <= limits.max_depth);
  check_position_equals(position, before);
}
