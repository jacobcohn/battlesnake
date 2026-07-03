#include <cstddef>
#include <initializer_list>
#include <vector>

#include <doctest/doctest.h>

#include "engine/move_execution.hpp"
#include "engine/square.hpp"

namespace {

void make_snake(Snake& snake, std::initializer_list<Square> head_to_tail, uint8_t health = 42) {
  const std::vector<Square> squares(head_to_tail);

  snake.reset();
  snake.head = squares.front();
  snake.tail = squares.back();
  snake.length = static_cast<uint8_t>(squares.size());
  snake.health = health;
  snake.alive = true;

  for (std::size_t i = squares.size() - 1; i > 0; --i) {
    snake.next[static_cast<std::size_t>(squares[i])] = squares[i - 1];
  }
}

void make_three_segment_snake(Snake& snake, Square tail, Square middle, Square head) {
  make_snake(snake, {head, middle, tail});
}

void check_snake_reset(const Snake& snake) {
  CHECK(snake.head == INVALID_SQUARE);
  CHECK(snake.tail == INVALID_SQUARE);
  CHECK(snake.length == 0);
  CHECK(snake.health == 100);
  CHECK_FALSE(snake.alive);
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

} // namespace

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

TEST_CASE("make_move removes snake that moves off board") {
  Position position;
  position.reset();

  make_three_segment_snake(position.snakes[0], idx(0, 0), idx(0, 1), idx(0, 2));
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.occupied.empty());
  CHECK(position.turn == 1);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move removes starving snake") {
  Position position;
  position.reset();

  make_three_segment_snake(position.snakes[0], idx(4, 4), idx(4, 5), idx(4, 6));
  position.snakes[0].health = 1;
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.occupied.empty());

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move lets health one snake survive by eating") {
  Position position;
  position.reset();

  const Square new_head = idx(5, 7);
  make_three_segment_snake(position.snakes[0], idx(5, 4), idx(5, 5), idx(5, 6));
  position.snakes[0].health = 1;
  position.snake_count = 1;
  position.food.set(new_head);
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].health == 100);
  CHECK(position.snakes[0].length == 4);
  CHECK(position.snakes[0].head == new_head);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move removes snake on self collision") {
  Position position;
  position.reset();

  make_snake(position.snakes[0], {idx(3, 2), idx(3, 3), idx(2, 3), idx(2, 2)});
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.occupied.empty());

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move allows moving into own vacated tail") {
  Position position;
  position.reset();

  const Square old_tail = idx(2, 2);
  const Square new_head = old_tail;
  make_snake(position.snakes[0], {idx(3, 2), idx(3, 3), idx(2, 3), old_tail});
  position.snake_count = 1;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].head == new_head);
  CHECK(position.snakes[0].length == 4);
  CHECK(position.occupied.count() == 4);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move removes snake on opponent body collision") {
  Position position;
  position.reset();

  make_snake(position.snakes[0], {idx(1, 1), idx(0, 1), idx(0, 0)});
  make_snake(position.snakes[1], {idx(2, 2), idx(2, 1), idx(2, 0)});
  position.snake_count = 2;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Up;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.snakes[1].alive);
  CHECK(position.occupied.count() == 3);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move allows moving into opponent vacated tail") {
  Position position;
  position.reset();

  const Square tail = idx(3, 5);
  make_snake(position.snakes[0], {idx(2, 5), idx(1, 5), idx(0, 5)});
  make_snake(position.snakes[1], {idx(5, 5), idx(4, 5), tail});
  position.snake_count = 2;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Right;

  Undo undo;
  make_move(position, joint, undo);

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].head == tail);
  CHECK(position.snakes[1].alive);
  CHECK(position.occupied.count() == 6);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move removes snake moving into growing opponent tail") {
  Position position;
  position.reset();

  const Square tail = idx(3, 5);
  const Square opponent_food = idx(6, 5);
  make_snake(position.snakes[0], {idx(2, 5), idx(1, 5), idx(0, 5)});
  make_snake(position.snakes[1], {idx(5, 5), idx(4, 5), tail});
  position.snake_count = 2;
  position.food.set(opponent_food);
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Right;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.snakes[1].alive);
  CHECK(position.snakes[1].length == 4);
  CHECK_FALSE(position.food.test(opponent_food));
  CHECK(position.occupied.count() == 4);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move removes equal length head-to-head snakes") {
  Position position;
  position.reset();

  make_snake(position.snakes[0], {idx(4, 5), idx(3, 5), idx(2, 5)});
  make_snake(position.snakes[1], {idx(6, 5), idx(7, 5), idx(8, 5)});
  position.snake_count = 2;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  check_snake_reset(position.snakes[1]);
  CHECK(position.occupied.empty());

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move keeps longer snake after head-to-head") {
  Position position;
  position.reset();

  make_snake(position.snakes[0], {idx(4, 5), idx(3, 5), idx(2, 5)});
  make_snake(position.snakes[1], {idx(6, 5), idx(7, 5), idx(8, 5), idx(9, 5)});
  position.snake_count = 2;
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.snakes[1].alive);
  CHECK(position.snakes[1].head == idx(5, 5));
  CHECK(position.snakes[1].length == 4);
  CHECK(position.occupied.count() == 4);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}

TEST_CASE("make_move uses old length for food head-to-head") {
  Position position;
  position.reset();

  const Square food = idx(5, 5);
  make_snake(position.snakes[0], {idx(4, 5), idx(3, 5), idx(2, 5)});
  make_snake(position.snakes[1], {idx(6, 5), idx(7, 5), idx(8, 5), idx(9, 5)});
  position.snake_count = 2;
  position.food.set(food);
  position.rebuild_occupied();

  JointMove joint;
  joint.moves[0] = Move::Right;
  joint.moves[1] = Move::Left;

  Undo undo;
  make_move(position, joint, undo);

  check_snake_reset(position.snakes[0]);
  CHECK(position.snakes[1].alive);
  CHECK(position.snakes[1].head == food);
  CHECK(position.snakes[1].length == 5);
  CHECK(position.snakes[1].health == 100);
  CHECK_FALSE(position.food.test(food));
  CHECK(position.occupied.count() == 5);

  unmake_move(position, undo);
  check_position_restored_from_undo(position, undo);
}
