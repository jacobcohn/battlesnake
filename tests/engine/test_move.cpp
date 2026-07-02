#include <cstddef>

#include <doctest/doctest.h>

#include "engine/constants.hpp"
#include "engine/move.hpp"

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
