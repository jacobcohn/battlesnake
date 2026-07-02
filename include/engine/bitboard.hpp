#pragma once

#include <bit>
#include <cassert>
#include <cstdint>

#include "constants.hpp"
#include "square.hpp"

struct BitBoard {
  uint64_t lo = 0;
  uint64_t hi = 0;

  static constexpr uint64_t HI_USED_MASK = (uint64_t{1} << (BOARD_SQUARES - 64)) - 1;

  [[nodiscard]] static constexpr BitBoard single(Square sq) noexcept {
    BitBoard bb;
    bb.set(sq);
    return bb;
  }

  constexpr void set(Square sq) noexcept {
    assert(sq >= 0 && sq < BOARD_SQUARES);
    if (sq < 64) {
      lo |= uint64_t{1} << sq;
    } else {
      hi |= uint64_t{1} << (sq - 64);
    }
  }

  constexpr void clear(Square sq) noexcept {
    assert(sq >= 0 && sq < BOARD_SQUARES);
    if (sq < 64) {
      lo &= ~(uint64_t{1} << sq);
    } else {
      hi &= ~(uint64_t{1} << (sq - 64));
    }
  }

  [[nodiscard]] constexpr bool test(Square sq) const noexcept {
    assert(sq >= 0 && sq < BOARD_SQUARES);
    if (sq < 64) {
      return (lo & (uint64_t{1} << sq)) != 0;
    }
    return (hi & (uint64_t{1} << (sq - 64))) != 0;
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return lo == 0 && hi == 0;
  }

  [[nodiscard]] constexpr int count() const noexcept {
    return std::popcount(lo) + std::popcount(hi);
  }

  constexpr BitBoard& operator|=(BitBoard rhs) noexcept {
    lo |= rhs.lo;
    hi |= rhs.hi;
    return *this;
  }

  constexpr BitBoard& operator&=(BitBoard rhs) noexcept {
    lo &= rhs.lo;
    hi &= rhs.hi;
    return *this;
  }

  constexpr BitBoard& operator^=(BitBoard rhs) noexcept {
    lo ^= rhs.lo;
    hi ^= rhs.hi;
    return *this;
  }

  [[nodiscard]] friend constexpr bool operator==(BitBoard lhs, BitBoard rhs) noexcept {
    return lhs.lo == rhs.lo && lhs.hi == rhs.hi;
  }

  [[nodiscard]] friend constexpr bool operator!=(BitBoard lhs, BitBoard rhs) noexcept {
    return !(lhs == rhs);
  }
};

inline constexpr BitBoard BOARD_MASK{~uint64_t{0}, BitBoard::HI_USED_MASK};

[[nodiscard]] constexpr BitBoard operator|(BitBoard lhs, BitBoard rhs) noexcept {
  lhs |= rhs;
  return lhs;
}

[[nodiscard]] constexpr BitBoard operator&(BitBoard lhs, BitBoard rhs) noexcept {
  lhs &= rhs;
  return lhs;
}

[[nodiscard]] constexpr BitBoard operator^(BitBoard lhs, BitBoard rhs) noexcept {
  lhs ^= rhs;
  return lhs;
}

[[nodiscard]] constexpr BitBoard operator~(BitBoard bb) noexcept {
  return BitBoard{~bb.lo, (~bb.hi) & BitBoard::HI_USED_MASK};
}
