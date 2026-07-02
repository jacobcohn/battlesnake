#include <cstdint>

#include <doctest/doctest.h>

#include "engine/bitboard.hpp"
#include "engine/constants.hpp"
#include "engine/square.hpp"

TEST_CASE("BitBoard starts empty") {
  const BitBoard board;

  CHECK(board.empty());
  CHECK(board.count() == 0);
  CHECK(board.low == 0);
  CHECK(board.high == 0);
}

TEST_CASE("BitBoard sets, tests, clears, and counts squares") {
  BitBoard board;

  board.set(0);
  board.set(10);
  board.set(64);
  board.set(120);

  CHECK_FALSE(board.empty());
  CHECK(board.count() == 4);
  CHECK(board.test(0));
  CHECK(board.test(10));
  CHECK(board.test(64));
  CHECK(board.test(120));
  CHECK_FALSE(board.test(1));
  CHECK_FALSE(board.test(119));

  board.clear(10);
  board.clear(64);

  CHECK(board.count() == 2);
  CHECK(board.test(0));
  CHECK_FALSE(board.test(10));
  CHECK_FALSE(board.test(64));
  CHECK(board.test(120));
}

TEST_CASE("BitBoard handles low/high storage boundary") {
  BitBoard board;

  board.set(63);
  board.set(64);

  CHECK(board.count() == 2);
  CHECK(board.test(63));
  CHECK(board.test(64));
  CHECK(board.low == (uint64_t{1} << 63));
  CHECK(board.high == uint64_t{1});

  board.clear(63);

  CHECK(board.count() == 1);
  CHECK_FALSE(board.test(63));
  CHECK(board.test(64));
  CHECK(board.low == 0);
  CHECK(board.high == uint64_t{1});
}

TEST_CASE("BitBoard creates single-square boards") {
  const Square square = idx(5, 5);
  const BitBoard board = BitBoard::single(square);

  CHECK_FALSE(board.empty());
  CHECK(board.count() == 1);
  CHECK(board.test(square));
  CHECK_FALSE(board.test(square - 1));
  CHECK_FALSE(board.test(square + 1));
}

TEST_CASE("BitBoard combines boards with bitwise operators") {
  BitBoard a;
  a.set(0);
  a.set(64);

  BitBoard b;
  b.set(64);
  b.set(120);

  const BitBoard union_board = a | b;
  CHECK(union_board.count() == 3);
  CHECK(union_board.test(0));
  CHECK(union_board.test(64));
  CHECK(union_board.test(120));

  const BitBoard intersection_board = a & b;
  CHECK(intersection_board.count() == 1);
  CHECK(intersection_board.test(64));
  CHECK_FALSE(intersection_board.test(0));
  CHECK_FALSE(intersection_board.test(120));

  const BitBoard xor_board = a ^ b;
  CHECK(xor_board.count() == 2);
  CHECK(xor_board.test(0));
  CHECK_FALSE(xor_board.test(64));
  CHECK(xor_board.test(120));
}

TEST_CASE("BitBoard compound operators update boards in place") {
  BitBoard board;
  board.set(0);

  BitBoard other;
  other.set(64);
  other.set(120);

  board |= other;
  CHECK(board.count() == 3);
  CHECK(board.test(0));
  CHECK(board.test(64));
  CHECK(board.test(120));

  BitBoard mask;
  mask.set(64);
  mask.set(120);

  board &= mask;
  CHECK(board.count() == 2);
  CHECK_FALSE(board.test(0));
  CHECK(board.test(64));
  CHECK(board.test(120));

  board ^= BitBoard::single(120);
  CHECK(board.count() == 1);
  CHECK(board.test(64));
  CHECK_FALSE(board.test(120));
}

TEST_CASE("BitBoard complement is limited to valid board squares") {
  BitBoard board;
  board.set(0);
  board.set(120);

  const BitBoard inverse = ~board;

  CHECK(inverse.count() == BOARD_SQUARES - 2);
  CHECK_FALSE(inverse.test(0));
  CHECK(inverse.test(1));
  CHECK(inverse.test(119));
  CHECK_FALSE(inverse.test(120));
  CHECK((inverse.high & ~BitBoard::HIGH_USED_MASK) == 0);
}

TEST_CASE("BOARD_MASK contains exactly all valid board squares") {
  CHECK(BOARD_MASK.count() == BOARD_SQUARES);
  CHECK(BOARD_MASK.test(0));
  CHECK(BOARD_MASK.test(63));
  CHECK(BOARD_MASK.test(64));
  CHECK(BOARD_MASK.test(120));
  CHECK(BOARD_MASK.low == ~uint64_t{0});
  CHECK(BOARD_MASK.high == BitBoard::HIGH_USED_MASK);
}

TEST_CASE("BitBoard equality compares both storage words") {
  BitBoard a;
  a.set(0);
  a.set(120);

  BitBoard b;
  b.set(0);
  b.set(120);

  BitBoard c;
  c.set(0);
  c.set(64);

  CHECK(a == b);
  CHECK_FALSE(a != b);
  CHECK(a != c);
  CHECK_FALSE(a == c);
}
