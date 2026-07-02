#include <doctest/doctest.h>

#include "engine/constants.hpp"
#include "engine/square.hpp"

TEST_CASE("square constants match board assumptions") {
  CHECK(BOARD_WIDTH == 11);
  CHECK(BOARD_HEIGHT == 11);
  CHECK(BOARD_SQUARES == 121);
  CHECK(INVALID_SQUARE == -1);
}

TEST_CASE("idx maps coordinates to row-major squares") {
  CHECK(idx(0, 0) == 0);
  CHECK(idx(1, 0) == 1);
  CHECK(idx(10, 0) == 10);

  CHECK(idx(0, 1) == 11);
  CHECK(idx(1, 1) == 12);
  CHECK(idx(10, 1) == 21);

  CHECK(idx(0, 10) == 110);
  CHECK(idx(10, 10) == 120);
}

TEST_CASE("x_of maps squares to x coordinates") {
  CHECK(x_of(0) == 0);
  CHECK(x_of(1) == 1);
  CHECK(x_of(10) == 10);
  CHECK(x_of(11) == 0);
  CHECK(x_of(12) == 1);
  CHECK(x_of(120) == 10);
}

TEST_CASE("y_of maps squares to y coordinates") {
  CHECK(y_of(0) == 0);
  CHECK(y_of(10) == 0);
  CHECK(y_of(11) == 1);
  CHECK(y_of(21) == 1);
  CHECK(y_of(110) == 10);
  CHECK(y_of(120) == 10);
}
