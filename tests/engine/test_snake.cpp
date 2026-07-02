#include <cstddef>

#include <doctest/doctest.h>

#include "engine/constants.hpp"
#include "engine/snake.hpp"
#include "engine/square.hpp"

namespace {

void check_snake_is_reset(const Snake& snake) {
  CHECK(snake.head == INVALID_SQUARE);
  CHECK(snake.tail == INVALID_SQUARE);
  CHECK(static_cast<int>(snake.length) == 0);
  CHECK(static_cast<int>(snake.health) == 100);
  CHECK_FALSE(snake.alive);

  for (std::size_t i = 0; i < snake.next.size(); ++i) {
    CAPTURE(i);
    CHECK(snake.next[i] == INVALID_SQUARE);
  }
}

}

TEST_CASE("Snake default construction creates a reset snake") {
  const Snake snake;

  check_snake_is_reset(snake);
}

TEST_CASE("Snake reset clears all mutable state") {
  Snake snake;

  snake.head = idx(5, 5);
  snake.tail = idx(5, 3);
  snake.length = 3;
  snake.health = 42;
  snake.alive = true;
  snake.next[static_cast<std::size_t>(idx(5, 3))] = idx(5, 4);
  snake.next[static_cast<std::size_t>(idx(5, 4))] = idx(5, 5);
  snake.next[static_cast<std::size_t>(idx(0, 0))] = idx(1, 0);

  snake.reset();

  check_snake_is_reset(snake);
}
