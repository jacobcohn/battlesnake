#include <cstddef>

#include <doctest/doctest.h>

#include "engine/move_execution.hpp"
#include "engine/square.hpp"

namespace {

void make_three_segment_snake(Snake& snake, Square tail, Square middle, Square head) {
  snake.reset();
  snake.tail = tail;
  snake.head = head;
  snake.length = 3;
  snake.health = 42;
  snake.alive = true;
  snake.next[static_cast<std::size_t>(tail)] = middle;
  snake.next[static_cast<std::size_t>(middle)] = head;
}

}

TEST_CASE("unmake_move restores full position state from undo") {
  Undo undo;

  const Square tail = idx(1, 1);
  const Square middle = idx(1, 2);
  const Square head = idx(1, 3);
  make_three_segment_snake(undo.snakes[0], tail, middle, head);

  undo.snake_count = 1;
  undo.you = 0;
  undo.turn = 19;
  undo.occupied.set(tail);
  undo.occupied.set(middle);
  undo.occupied.set(head);
  undo.food.set(idx(5, 5));
  undo.hazards.set(idx(9, 9));
  undo.exact_hash = 1234;
  undo.space_hash = 5678;

  Position position;
  position.reset();
  position.snake_count = 2;
  position.you = 1;
  position.turn = 20;
  position.occupied.set(idx(0, 0));
  position.food.set(idx(0, 1));
  position.hazards.set(idx(0, 2));
  position.exact_hash = 111;
  position.space_hash = 222;

  unmake_move(position, undo);

  CHECK(position.snake_count == undo.snake_count);
  CHECK(position.you == undo.you);
  CHECK(position.turn == undo.turn);
  CHECK(position.occupied == undo.occupied);
  CHECK(position.food == undo.food);
  CHECK(position.hazards == undo.hazards);
  CHECK(position.exact_hash == undo.exact_hash);
  CHECK(position.space_hash == undo.space_hash);

  const Snake& snake = position.snakes[0];
  CHECK(snake.tail == tail);
  CHECK(snake.head == head);
  CHECK(snake.length == 3);
  CHECK(snake.health == 42);
  CHECK(snake.alive);
  CHECK(snake.next[static_cast<std::size_t>(tail)] == middle);
  CHECK(snake.next[static_cast<std::size_t>(middle)] == head);
}
