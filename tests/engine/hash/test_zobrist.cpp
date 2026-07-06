#include <array>
#include <cstddef>

#include <doctest/doctest.h>

#include "engine/core/position.hpp"
#include "engine/core/square.hpp"
#include "engine/hash/zobrist.hpp"
#include "engine/move/execution.hpp"

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

void finalize(Position& position) {
  position.build_occupied();
  engine::hash::initialize_hashes(position);
}

Position one_snake_position(std::initializer_list<Square> head_to_tail, uint8_t health = 100) {
  Position position;
  position.reset();
  position.snake_count = 1;
  position.you = 0;
  set_snake(position.snakes[0], head_to_tail, health);
  finalize(position);
  return position;
}

Position two_snake_position() {
  Position position;
  position.reset();
  position.snake_count = 2;
  position.you = 0;
  set_snake(position.snakes[0], {idx(5, 5), idx(5, 4), idx(5, 3)}, 90);
  set_snake(position.snakes[1], {idx(8, 8), idx(8, 7), idx(8, 6)}, 80);
  finalize(position);
  return position;
}

JointMove moves(Move first, Move second = Move::Up) {
  JointMove joint;
  joint.moves[0] = first;
  joint.moves[1] = second;
  return joint;
}

void check_stored_hashes_match_recomputed(const Position& position) {
  CHECK(position.space_hash == engine::hash::compute_space_hash(position));
  CHECK(position.exact_hash == engine::hash::compute_exact_hash(position));
}

}

TEST_CASE("initialize_hashes stores recomputed Zobrist hashes") {
  Position position = two_snake_position();
  position.food.set(idx(2, 2));
  position.food.set(idx(9, 1));
  position.turn = 37;
  engine::hash::initialize_hashes(position);

  check_stored_hashes_match_recomputed(position);
}

TEST_CASE("make_move recomputes Zobrist hashes and unmake restores them exactly") {
  Position position = two_snake_position();
  position.food.set(idx(5, 6));
  engine::hash::initialize_hashes(position);

  const uint64_t old_space_hash = position.space_hash;
  const uint64_t old_exact_hash = position.exact_hash;

  Undo undo = make_move(position, moves(Move::Up, Move::Up));

  check_stored_hashes_match_recomputed(position);
  CHECK(position.space_hash != old_space_hash);
  CHECK(position.exact_hash != old_exact_hash);

  unmake_move(position, undo);

  CHECK(position.space_hash == old_space_hash);
  CHECK(position.exact_hash == old_exact_hash);
  check_stored_hashes_match_recomputed(position);
}

TEST_CASE("Zobrist hashes update correctly when a snake dies") {
  Position position = one_snake_position({idx(5, 5), idx(5, 4), idx(5, 3)}, 1);
  const uint64_t old_space_hash = position.space_hash;
  const uint64_t old_exact_hash = position.exact_hash;

  Undo undo = make_move(position, moves(Move::Up));

  CHECK_FALSE(position.snakes[0].alive);
  check_stored_hashes_match_recomputed(position);
  CHECK(position.space_hash != old_space_hash);
  CHECK(position.exact_hash != old_exact_hash);

  unmake_move(position, undo);
  CHECK(position.space_hash == old_space_hash);
  CHECK(position.exact_hash == old_exact_hash);
}

TEST_CASE("exact hash changes for turn health food body order and you") {
  Position base = two_snake_position();
  const uint64_t base_exact = base.exact_hash;

  Position turn_changed = base;
  ++turn_changed.turn;
  engine::hash::initialize_hashes(turn_changed);
  CHECK(turn_changed.exact_hash != base_exact);

  Position health_changed = base;
  --health_changed.snakes[0].health;
  engine::hash::initialize_hashes(health_changed);
  CHECK(health_changed.exact_hash != base_exact);

  Position food_changed = base;
  food_changed.food.set(idx(0, 0));
  engine::hash::initialize_hashes(food_changed);
  CHECK(food_changed.exact_hash != base_exact);

  Position you_changed = base;
  you_changed.you = 1;
  engine::hash::initialize_hashes(you_changed);
  CHECK(you_changed.exact_hash != base_exact);

  Position order_a = one_snake_position({idx(1, 1), idx(1, 0), idx(0, 0), idx(0, 1)});
  Position order_b = one_snake_position({idx(1, 1), idx(0, 1), idx(0, 0), idx(1, 0)});
  CHECK(order_a.snakes[0].occupied == order_b.snakes[0].occupied);
  CHECK(order_a.exact_hash != order_b.exact_hash);
}

TEST_CASE("space hash ignores turn health food and you") {
  Position base = two_snake_position();
  const uint64_t base_space = base.space_hash;

  Position changed = base;
  changed.turn = static_cast<uint16_t>(changed.turn + 19);
  changed.snakes[0].health = static_cast<uint8_t>(changed.snakes[0].health - 7);
  changed.food.set(idx(0, 0));
  changed.food.set(idx(10, 10));
  changed.you = 1;
  engine::hash::initialize_hashes(changed);

  CHECK(changed.space_hash == base_space);
  CHECK(changed.exact_hash != base.exact_hash);
}

TEST_CASE("space hash changes for directed body path changes") {
  Position order_a = one_snake_position({idx(1, 1), idx(1, 0), idx(0, 0), idx(0, 1)});
  Position order_b = one_snake_position({idx(1, 1), idx(0, 1), idx(0, 0), idx(1, 0)});

  CHECK(order_a.snakes[0].occupied == order_b.snakes[0].occupied);
  CHECK(order_a.space_hash != order_b.space_hash);
}

TEST_CASE("strict edge-only space hash gives length one snakes no body contribution") {
  Position position = one_snake_position({idx(4, 4)});

  CHECK(position.space_hash == 0);
  CHECK(position.exact_hash != 0);
}
