#include <cstddef>

#include <doctest/doctest.h>

#include "engine/core/constants.hpp"
#include "engine/core/move.hpp"
#include "engine/core/position.hpp"
#include "engine/move/generation.hpp"

namespace {

Position make_position(int snake_count) {
  Position position;
  position.reset();
  position.snake_count = static_cast<unsigned char>(snake_count);
  return position;
}

bool has_move(const engine::move::JointMoveList& list, Move first, Move second = Move::Up) {
  for (int i = 0; i < list.count; ++i) {
    const JointMove& joint = list.moves[static_cast<std::size_t>(i)];
    if (joint.moves[0] == first && joint.moves[1] == second) {
      return true;
    }
  }

  return false;
}

}

TEST_CASE("joint move generation creates one move for no snakes") {
  Position position = make_position(0);

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);

  CHECK(moves.count == 1);
  CHECK(moves.moves[0].moves[0] == Move::Up);
}

TEST_CASE("joint move generation uses only snakes below snake count") {
  Position position = make_position(1);
  position.snakes[0].alive = true;
  position.snakes[1].alive = true;

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);

  CHECK(moves.count == 4);
  CHECK(has_move(moves, Move::Up));
  CHECK(has_move(moves, Move::Down));
  CHECK(has_move(moves, Move::Left));
  CHECK(has_move(moves, Move::Right));

  for (int i = 0; i < moves.count; ++i) {
    CHECK(moves.moves[static_cast<std::size_t>(i)].moves[1] == Move::Up);
  }
}

TEST_CASE("joint move generation gives dead snakes one dummy move") {
  Position position = make_position(2);
  position.snakes[0].alive = true;
  position.snakes[1].alive = false;

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);

  CHECK(moves.count == 4);
  CHECK(has_move(moves, Move::Up, Move::Up));
  CHECK(has_move(moves, Move::Down, Move::Up));
  CHECK(has_move(moves, Move::Left, Move::Up));
  CHECK(has_move(moves, Move::Right, Move::Up));
}

TEST_CASE("joint move generation creates all combinations for alive snakes") {
  Position position = make_position(2);
  position.snakes[0].alive = true;
  position.snakes[1].alive = true;

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);

  CHECK(moves.count == 16);
  CHECK(has_move(moves, Move::Up, Move::Up));
  CHECK(has_move(moves, Move::Up, Move::Down));
  CHECK(has_move(moves, Move::Down, Move::Left));
  CHECK(has_move(moves, Move::Right, Move::Right));
}
