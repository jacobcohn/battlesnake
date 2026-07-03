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

void check_position_restored_from_undo(const Position& position, const Undo& undo) {
  for (std::size_t i = 0; i < position.snakes.size(); ++i) {
    CHECK(position.snakes[i].head == undo.snakes[i].head);
    CHECK(position.snakes[i].tail == undo.snakes[i].tail);
    CHECK(position.snakes[i].next == undo.snakes[i].next);
    CHECK(position.snakes[i].length == undo.snakes[i].length);
    CHECK(position.snakes[i].health == undo.snakes[i].health);
    CHECK(position.snakes[i].alive == undo.snakes[i].alive);
  }

  CHECK(position.snake_count == undo.snake_count);
  CHECK(position.you == undo.you);
  CHECK(position.turn == undo.turn);
  CHECK(position.occupied == undo.occupied);
  CHECK(position.food == undo.food);
  CHECK(position.hazards == undo.hazards);
  CHECK(position.exact_hash == undo.exact_hash);
  CHECK(position.space_hash == undo.space_hash);
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

TEST_CASE("make_move moves snake without growth") {
  Position position;
  position.reset();

  const Square old_tail = idx(1, 1);
  const Square middle = idx(1, 2);
  const Square old_head = idx(1, 3);
  const Square new_head = idx(1, 4);
  make_three_segment_snake(position.snakes[0], old_tail, middle, old_head);
  position.snakes[0].health = 42;
  position.snake_count = 1;
  position.turn = 7;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  const Snake& snake = position.snakes[0];
  CHECK(snake.alive);
  CHECK(snake.tail == middle);
  CHECK(snake.head == new_head);
  CHECK(snake.length == 3);
  CHECK(snake.health == 41);
  CHECK(snake.next[static_cast<std::size_t>(old_tail)] == INVALID_SQUARE);
  CHECK(snake.next[static_cast<std::size_t>(middle)] == old_head);
  CHECK(snake.next[static_cast<std::size_t>(old_head)] == new_head);
  CHECK(position.turn == 8);
  CHECK(position.occupied.count() == 3);
  CHECK_FALSE(position.occupied.test(old_tail));
  CHECK(position.occupied.test(middle));
  CHECK(position.occupied.test(old_head));
  CHECK(position.occupied.test(new_head));

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move grows snake on food") {
  Position position;
  position.reset();

  const Square tail = idx(2, 2);
  const Square middle = idx(2, 3);
  const Square old_head = idx(2, 4);
  const Square new_head = idx(2, 5);
  make_three_segment_snake(position.snakes[0], tail, middle, old_head);
  position.snakes[0].health = 17;
  position.snake_count = 1;
  position.food.set(new_head);
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  const Snake& snake = position.snakes[0];
  CHECK(snake.alive);
  CHECK(snake.tail == tail);
  CHECK(snake.head == new_head);
  CHECK(snake.length == 4);
  CHECK(snake.health == 100);
  CHECK(snake.next[static_cast<std::size_t>(tail)] == middle);
  CHECK(snake.next[static_cast<std::size_t>(middle)] == old_head);
  CHECK(snake.next[static_cast<std::size_t>(old_head)] == new_head);
  CHECK_FALSE(position.food.test(new_head));
  CHECK(position.occupied.count() == 4);
  CHECK(position.occupied.test(tail));
  CHECK(position.occupied.test(middle));
  CHECK(position.occupied.test(old_head));
  CHECK(position.occupied.test(new_head));

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move ignores dead snakes") {
  Position position;
  position.reset();

  const Square tail = idx(3, 3);
  const Square middle = idx(3, 4);
  const Square head = idx(3, 5);
  make_three_segment_snake(position.snakes[0], tail, middle, head);
  position.snakes[0].alive = false;
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  const Snake& snake = position.snakes[0];
  CHECK_FALSE(snake.alive);
  CHECK(snake.tail == tail);
  CHECK(snake.head == head);
  CHECK(snake.length == 3);
  CHECK(snake.health == 42);
  CHECK(position.occupied.empty());
  CHECK(position.turn == 1);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move kills snake that moves off board") {
  Position position;
  position.reset();

  const Square tail = idx(0, 0);
  const Square middle = idx(0, 1);
  const Square head = idx(0, 2);
  make_three_segment_snake(position.snakes[0], tail, middle, head);
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.occupied.empty());
  CHECK(position.turn == 1);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}
