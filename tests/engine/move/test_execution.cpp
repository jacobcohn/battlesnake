#include <array>
#include <cstddef>

#include <doctest/doctest.h>

#include "engine/move/execution.hpp"
#include "engine/core/square.hpp"

namespace {

void set_snake(Snake& snake, std::initializer_list<Square> head_to_tail, uint8_t health = 100) {
  snake.reset();

  std::array<Square, BOARD_SQUARES> squares{};
  std::size_t count = 0;
  for (Square square : head_to_tail) {
    squares[count++] = square;
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

void init_occupied(Position& position) {
  position.occupied = BitBoard{};
  position.build_occupied();
}

Position one_snake_position(std::initializer_list<Square> head_to_tail, uint8_t health = 100) {
  Position position;
  position.reset();
  position.snake_count = 1;
  position.you = 0;
  set_snake(position.snakes[0], head_to_tail, health);
  init_occupied(position);
  return position;
}

JointMove moves(Move a, Move b = Move::Up) {
  JointMove joint;
  joint.moves[0] = a;
  joint.moves[1] = b;
  return joint;
}

void check_body(const Snake& snake, std::initializer_list<Square> tail_to_head) {
  Square current = snake.tail;
  int visited = 0;
  for (Square expected : tail_to_head) {
    CHECK(current == expected);
    current = snake.next[static_cast<std::size_t>(current)];
    ++visited;
  }
  CHECK(visited == snake.length);
  CHECK(current == INVALID_SQUARE);
}

void check_unmake_restores(Position& position, const Position& before, const Undo& undo) {
  unmake_move(position, undo);
  CHECK(position.occupied == before.occupied);
  CHECK(position.food == before.food);
  CHECK(position.snake_count == before.snake_count);
  CHECK(position.you == before.you);
  CHECK(position.turn == before.turn);
  CHECK(position.exact_hash == before.exact_hash);
  CHECK(position.space_hash == before.space_hash);

  for (int i = 0; i < before.snake_count; ++i) {
    const Snake& actual = position.snakes[static_cast<std::size_t>(i)];
    const Snake& expected = before.snakes[static_cast<std::size_t>(i)];
    CHECK(actual.head == expected.head);
    CHECK(actual.tail == expected.tail);
    CHECK(actual.length == expected.length);
    CHECK(actual.stacked_tail_segments == expected.stacked_tail_segments);
    CHECK(actual.health == expected.health);
    CHECK(actual.alive == expected.alive);
    CHECK(actual.next == expected.next);
    CHECK(actual.occupied == expected.occupied);
  }
}

}

TEST_CASE("make_move updates head tail body health occupied and undo restores") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(5, 3)}, 90);
  const Position before = position;
  Undo undo = make_move(position, moves(Move::Up));

  const Snake& snake = position.snakes[0];
  CHECK(snake.alive);
  CHECK(snake.head == idx(5, 6));
  CHECK(snake.tail == idx(5, 4));
  CHECK(snake.length == 3);
  CHECK(snake.health == 89);
  check_body(snake, {idx(5, 4), idx(5, 5), idx(5, 6)});
  CHECK_FALSE(snake.occupied.test(idx(5, 3)));
  CHECK(snake.occupied.test(idx(5, 6)));
  CHECK(snake.occupied.count() == 3);
  CHECK_FALSE(position.occupied.test(idx(5, 3)));
  CHECK(position.occupied.test(idx(5, 4)));
  CHECK(position.occupied.test(idx(5, 5)));
  CHECK(position.occupied.test(idx(5, 6)));
  CHECK(position.occupied.count() == 3);
  CHECK(position.turn == before.turn + 1);

  check_unmake_restores(position, before, undo);
}

TEST_CASE("stacked starting body keeps tail in place until body unstacks") {
  Position position;
  position.reset();
  position.snake_count = 1;
  position.you = 0;

  Snake& snake = position.snakes[0];
  snake.reset();
  snake.head = idx(5, 5);
  snake.tail = idx(5, 5);
  snake.length = 3;
  snake.stacked_tail_segments = 2;
  snake.health = 100;
  snake.alive = true;
  snake.occupied.set(idx(5, 5));
  init_occupied(position);

  Position before = position;
  Undo undo = make_move(position, moves(Move::Up));
  CHECK(position.snakes[0].head == idx(5, 6));
  CHECK(position.snakes[0].tail == idx(5, 5));
  CHECK(position.snakes[0].length == 3);
  CHECK(position.snakes[0].stacked_tail_segments == 1);
  CHECK(position.snakes[0].occupied.count() == 2);
  CHECK(position.snakes[0].occupied.test(idx(5, 5)));
  CHECK(position.snakes[0].occupied.test(idx(5, 6)));
  CHECK_FALSE(undo.removed_tail[0]);
  check_unmake_restores(position, before, undo);

  undo = make_move(position, moves(Move::Up));
  before = position;
  undo = make_move(position, moves(Move::Up));
  CHECK(position.snakes[0].head == idx(5, 7));
  CHECK(position.snakes[0].tail == idx(5, 5));
  CHECK(position.snakes[0].stacked_tail_segments == 0);
  CHECK(position.snakes[0].occupied.count() == 3);
  CHECK_FALSE(undo.removed_tail[0]);

  before = position;
  undo = make_move(position, moves(Move::Up));
  CHECK(position.snakes[0].head == idx(5, 8));
  CHECK(position.snakes[0].tail == idx(5, 6));
  CHECK(position.snakes[0].occupied.count() == 3);
  CHECK_FALSE(position.snakes[0].occupied.test(idx(5, 5)));
  CHECK(undo.removed_tail[0]);
  check_unmake_restores(position, before, undo);
}

TEST_CASE("moving into a stacked tail before it vacates is a collision") {
  Position position;
  position.reset();
  position.snake_count = 1;
  position.you = 0;

  Snake& snake = position.snakes[0];
  snake.reset();
  snake.head = idx(5, 5);
  snake.tail = idx(5, 5);
  snake.length = 3;
  snake.stacked_tail_segments = 2;
  snake.health = 100;
  snake.alive = true;
  snake.occupied.set(idx(5, 5));
  init_occupied(position);

  Undo undo = make_move(position, moves(Move::Up));
  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].stacked_tail_segments == 1);
  CHECK_FALSE(undo.removed_tail[0]);

  const Position before_collision = position;
  undo = make_move(position, moves(Move::Down));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK_FALSE(undo.removed_tail[0]);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before_collision, undo);
}

TEST_CASE("food grows snake resets health removes food and keeps tail occupied") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(5, 3)}, 12);
  position.food.set(idx(5, 6));
  const Position before = position;
  Undo undo = make_move(position, moves(Move::Up));

  const Snake& snake = position.snakes[0];
  CHECK(snake.alive);
  CHECK(snake.length == 4);
  CHECK(snake.health == 100);
  CHECK_FALSE(position.food.test(idx(5, 6)));
  check_body(snake, {idx(5, 3), idx(5, 4), idx(5, 5), idx(5, 6)});
  CHECK(snake.occupied.count() == 4);
  CHECK(snake.occupied.test(idx(5, 3)));
  CHECK(position.occupied.count() == 4);
  CHECK(position.occupied.test(idx(5, 3)));

  check_unmake_restores(position, before, undo);
}

TEST_CASE("snake dies by starvation and occupied body is removed") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(5, 3)}, 1);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Up));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.snakes[0].length == 3);
  CHECK(position.snakes[0].occupied.count() == 3);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before, undo);
}

TEST_CASE("snake dies out of bounds and occupied body is removed") {
  Position position = one_snake_position({idx(0, 0)}, 100);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Left));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.snakes[0].occupied.count() == 1);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before, undo);
}

TEST_CASE("self collision kills snake") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(4, 4), idx(4, 5)}, 100);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Down));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before, undo);
}

TEST_CASE("moving into own vacated tail is allowed") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(4, 4), idx(4, 5)}, 100);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Left));

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].head == idx(4, 5));
  CHECK(position.snakes[0].tail == idx(4, 4));
  CHECK(position.occupied.count() == 4);

  check_unmake_restores(position, before, undo);
}

TEST_CASE("moving into own tail while eating is a collision") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(4, 4), idx(4, 5)}, 100);
  position.food.set(idx(4, 5));
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Left));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before, undo);
}

TEST_CASE("collision with another body kills snake") {
  Position position;
  position.reset();
  position.snake_count = 2;
  set_snake(position.snakes[0], {idx(5, 5), idx(5, 4), idx(5, 3)}, 100);
  set_snake(position.snakes[1], {idx(7, 5), idx(6, 5), idx(6, 4)}, 100);
  init_occupied(position);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Right, Move::Up));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK(position.snakes[1].alive);
  CHECK_FALSE(position.occupied.test(idx(5, 5)));
  CHECK(position.occupied.test(idx(7, 6)));

  check_unmake_restores(position, before, undo);
}

TEST_CASE("moving into another snake vacated tail is allowed") {
  Position position;
  position.reset();
  position.snake_count = 2;
  set_snake(position.snakes[0], {idx(5, 5), idx(5, 4), idx(5, 3)}, 100);
  set_snake(position.snakes[1], {idx(6, 3), idx(6, 4), idx(6, 5)}, 100);
  init_occupied(position);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Right, Move::Right));

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].head == idx(6, 5));
  CHECK(position.snakes[1].alive);
  CHECK(position.occupied.test(idx(6, 5)));

  check_unmake_restores(position, before, undo);
}

TEST_CASE("equal length head to head kills both") {
  Position position;
  position.reset();
  position.snake_count = 2;
  set_snake(position.snakes[0], {idx(4, 5), idx(4, 4), idx(4, 3)}, 100);
  set_snake(position.snakes[1], {idx(6, 5), idx(6, 4), idx(6, 3)}, 100);
  init_occupied(position);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Right, Move::Left));

  CHECK_FALSE(position.snakes[0].alive);
  CHECK_FALSE(position.snakes[1].alive);
  CHECK(position.occupied.empty());

  check_unmake_restores(position, before, undo);
}

TEST_CASE("longer snake survives head to head") {
  Position position;
  position.reset();
  position.snake_count = 2;
  set_snake(position.snakes[0], {idx(4, 5), idx(4, 4), idx(4, 3), idx(3, 3)}, 100);
  set_snake(position.snakes[1], {idx(6, 5), idx(6, 4), idx(6, 3)}, 100);
  init_occupied(position);
  const Position before = position;

  Undo undo = make_move(position, moves(Move::Right, Move::Left));

  CHECK(position.snakes[0].alive);
  CHECK(position.snakes[0].length == 4);
  CHECK_FALSE(position.snakes[1].alive);
  CHECK(position.occupied.count() == 4);

  check_unmake_restores(position, before, undo);
}

TEST_CASE("repeated make and unmake cycles restore position") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(5, 3)}, 90);
  const Position before = position;

  for (int i = 0; i < 5; ++i) {
    Undo undo = make_move(position, moves(Move::Up));
    check_unmake_restores(position, before, undo);
  }
}
