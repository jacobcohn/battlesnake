#include <cstddef>

#include <doctest/doctest.h>

#include "engine/core/constants.hpp"
#include "engine/core/move.hpp"
#include "engine/core/square.hpp"
#include "engine/move/targets.hpp"

TEST_CASE("Move is compact") {
  CHECK(sizeof(Move) == 1);
}

TEST_CASE("JointMove stores moves for max snakes") {
  JointMove joint;

  joint.moves[0] = Move::Up;
  joint.moves[1] = Move::Left;

  CHECK(joint.moves[0] == Move::Up);
  CHECK(joint.moves[1] == Move::Left);
  CHECK(joint.moves.size() == static_cast<std::size_t>(MAX_SNAKES));
}

TEST_CASE("move_target returns adjacent squares") {
  const Square center = idx(5, 5);

  CHECK(move_target(center, Move::Up) == idx(5, 6));
  CHECK(move_target(center, Move::Down) == idx(5, 4));
  CHECK(move_target(center, Move::Left) == idx(4, 5));
  CHECK(move_target(center, Move::Right) == idx(6, 5));
}

TEST_CASE("move_target returns invalid when leaving the board") {
  CHECK(move_target(idx(0, 0), Move::Down) == INVALID_SQUARE);
  CHECK(move_target(idx(0, 0), Move::Left) == INVALID_SQUARE);
  CHECK(move_target(idx(BOARD_WIDTH - 1, BOARD_HEIGHT - 1), Move::Up) == INVALID_SQUARE);
  CHECK(move_target(idx(BOARD_WIDTH - 1, BOARD_HEIGHT - 1), Move::Right) == INVALID_SQUARE);
}

TEST_CASE("move_target allows valid edge moves") {
  CHECK(move_target(idx(0, 0), Move::Up) == idx(0, 1));
  CHECK(move_target(idx(0, 0), Move::Right) == idx(1, 0));
  CHECK(move_target(idx(BOARD_WIDTH - 1, BOARD_HEIGHT - 1), Move::Down) ==
    idx(BOARD_WIDTH - 1, BOARD_HEIGHT - 2));
  CHECK(move_target(idx(BOARD_WIDTH - 1, BOARD_HEIGHT - 1), Move::Left) ==
    idx(BOARD_WIDTH - 2, BOARD_HEIGHT - 1));
}

TEST_CASE("move_target handles invalid source") {
  CHECK(move_target(INVALID_SQUARE, Move::Up) == INVALID_SQUARE);
  CHECK(move_target(INVALID_SQUARE, Move::Down) == INVALID_SQUARE);
  CHECK(move_target(INVALID_SQUARE, Move::Left) == INVALID_SQUARE);
  CHECK(move_target(INVALID_SQUARE, Move::Right) == INVALID_SQUARE);
}
