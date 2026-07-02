#pragma once

#include <bit>
#include <cassert>
#include <cstdint>

#include "constants.hpp"
#include "square.hpp"

struct BitBoard {
  uint64_t low = 0;
  uint64_t high = 0;

  static constexpr uint64_t HIGH_USED_MASK = (uint64_t{1} << (BOARD_SQUARES - 64)) - 1;

  [[nodiscard]] static constexpr BitBoard single(Square square) noexcept {
    BitBoard bitBoard;
    bitBoard.set(square);
    return bitBoard;
  }

  constexpr void set(Square square) noexcept {
    assert(square >= 0 && square < BOARD_SQUARES);
    if (square < 64) {
      low |= uint64_t{1} << square;
    } else {
      high |= uint64_t{1} << (square - 64);
    }
  }

  constexpr void clear(Square square) noexcept {
    assert(square >= 0 && square < BOARD_SQUARES);
    if (square < 64) {
      low &= ~(uint64_t{1} << square);
    } else {
      high &= ~(uint64_t{1} << (square - 64));
    }
  }

  [[nodiscard]] constexpr bool test(Square square) const noexcept {
    assert(square >= 0 && square < BOARD_SQUARES);
    if (square < 64) {
      return (low & (uint64_t{1} << square)) != 0;
    }
    return (high & (uint64_t{1} << (square - 64))) != 0;
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return low == 0 && high == 0;
  }

  [[nodiscard]] constexpr int count() const noexcept {
    return std::popcount(low) + std::popcount(high);
  }

  constexpr BitBoard& operator|=(BitBoard rhs) noexcept {
    low |= rhs.low;
    high |= rhs.high;
    return *this;
  }

  constexpr BitBoard& operator&=(BitBoard rhs) noexcept {
    low &= rhs.low;
    high &= rhs.high;
    return *this;
  }

  constexpr BitBoard& operator^=(BitBoard rhs) noexcept {
    low ^= rhs.low;
    high ^= rhs.high;
    return *this;
  }

  [[nodiscard]] friend constexpr bool operator==(BitBoard lhs, BitBoard rhs) noexcept {
    return lhs.low == rhs.low && lhs.high == rhs.high;
  }

  [[nodiscard]] friend constexpr bool operator!=(BitBoard lhs, BitBoard rhs) noexcept {
    return !(lhs == rhs);
  }
};

inline constexpr BitBoard BOARD_MASK{~uint64_t{0}, BitBoard::HIGH_USED_MASK};

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
  return BitBoard{~bb.low, (~bb.high) & BitBoard::HIGH_USED_MASK};
}
