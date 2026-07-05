#include <cstddef>

#include <doctest/doctest.h>

#include "engine/core/bitboard.hpp"
#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"
#include "engine/core/square.hpp"

namespace {

void make_three_segment_snake(Snake& snake, Square tail, Square middle, Square head) {
  snake.reset();
  snake.tail = tail;
  snake.head = head;
  snake.length = 3;
  snake.health = 90;
  snake.alive = true;
  snake.occupied.set(tail);
  snake.occupied.set(middle);
  snake.occupied.set(head);
  snake.next[static_cast<std::size_t>(tail)] = middle;
  snake.next[static_cast<std::size_t>(middle)] = head;
}

}

TEST_CASE("Position reset clears all state") {
  Position position;

  make_three_segment_snake(position.snakes[0], idx(1, 1), idx(1, 2), idx(1, 3));
  position.snake_count = 1;
  position.you = 0;
  position.turn = 17;
  position.occupied.set(idx(1, 1));
  position.food.set(idx(5, 5));
  position.exact_hash = 123;
  position.space_hash = 456;

  position.reset();

  CHECK(position.snake_count == 0);
  CHECK(position.you == 0);
  CHECK(position.turn == 0);
  CHECK(position.occupied.empty());
  CHECK(position.food.empty());
  CHECK(position.exact_hash == 0);
  CHECK(position.space_hash == 0);

  for (const Snake& snake : position.snakes) {
    CHECK(snake.head == INVALID_SQUARE);
    CHECK(snake.tail == INVALID_SQUARE);
    CHECK(snake.length == 0);
    CHECK(snake.health == 100);
    CHECK_FALSE(snake.alive);
    CHECK(snake.occupied.empty());
  }
}

TEST_CASE("Position clear_bitboards clears only board masks") {
  Position position;

  position.snake_count = 1;
  position.you = 0;
  position.turn = 12;
  position.exact_hash = 111;
  position.space_hash = 222;
  position.occupied.set(idx(0, 0));
  position.food.set(idx(1, 1));

  position.clear_bitboards();

  CHECK(position.occupied.empty());
  CHECK(position.food.empty());
  CHECK(position.snake_count == 1);
  CHECK(position.you == 0);
  CHECK(position.turn == 12);
  CHECK(position.exact_hash == 111);
  CHECK(position.space_hash == 222);
}

TEST_CASE("Position build_occupied handles no snakes") {
  Position position;
  position.reset();
  position.occupied.set(idx(3, 3));
  position.build_occupied();

  CHECK(position.occupied.empty());
}

TEST_CASE("Position build_occupied marks every square in one alive snake") {
  Position position;
  position.reset();

  const Square tail = idx(4, 2);
  const Square middle = idx(4, 3);
  const Square head = idx(4, 4);
  make_three_segment_snake(position.snakes[0], tail, middle, head);
  position.snake_count = 1;
  position.build_occupied();

  CHECK(position.occupied.count() == 3);
  CHECK(position.occupied.test(tail));
  CHECK(position.occupied.test(middle));
  CHECK(position.occupied.test(head));
  CHECK_FALSE(position.occupied.test(idx(0, 0)));
}

TEST_CASE("Position build_occupied ignores dead and zero-length snakes") {
  Position position;
  position.reset();

  make_three_segment_snake(position.snakes[0], idx(0, 0), idx(0, 1), idx(0, 2));
  position.snakes[0].alive = false;

  make_three_segment_snake(position.snakes[1], idx(5, 5), idx(5, 6), idx(5, 7));
  position.snakes[1].length = 0;

  position.snake_count = 2;
  position.build_occupied();

  CHECK(position.occupied.empty());
}

TEST_CASE("Position build_occupied combines multiple alive snakes") {
  Position position;
  position.reset();

  const Square first_tail = idx(1, 1);
  const Square first_middle = idx(1, 2);
  const Square first_head = idx(1, 3);
  make_three_segment_snake(position.snakes[0], first_tail, first_middle, first_head);

  const Square second_tail = idx(9, 9);
  const Square second_middle = idx(8, 9);
  const Square second_head = idx(7, 9);
  make_three_segment_snake(position.snakes[1], second_tail, second_middle, second_head);

  position.snake_count = 2;
  position.build_occupied();

  CHECK(position.occupied.count() == 6);
  CHECK(position.occupied.test(first_tail));
  CHECK(position.occupied.test(first_middle));
  CHECK(position.occupied.test(first_head));
  CHECK(position.occupied.test(second_tail));
  CHECK(position.occupied.test(second_middle));
  CHECK(position.occupied.test(second_head));
}
