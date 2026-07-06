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

  constexpr bool pop_first_set_square(Square& square) noexcept {
    if (low != 0) {
      square = static_cast<Square>(std::countr_zero(low));
      low &= low - 1;
      return true;
    }

    if (high != 0) {
      square = static_cast<Square>(64 + std::countr_zero(high));
      high &= high - 1;
      return true;
    }

    square = INVALID_SQUARE;
    return false;
  }

  [[nodiscard]] static constexpr BitBoard file_mask(int x) noexcept {
    assert(x >= 0 && x < BOARD_WIDTH);

    BitBoard mask;
    for (int y = 0; y < BOARD_HEIGHT; ++y) {
      mask.set(idx(x, y));
    }
    return mask;
  }

  [[nodiscard]] constexpr BitBoard shift_left(int amount) const noexcept {
    assert(amount > 0 && amount < 64);

    BitBoard shifted;
    shifted.low = low << amount;
    shifted.high = ((high << amount) | (low >> (64 - amount))) & HIGH_USED_MASK;
    return shifted;
  }

  [[nodiscard]] constexpr BitBoard shift_right(int amount) const noexcept {
    assert(amount > 0 && amount < 64);

    BitBoard shifted;
    shifted.low = (low >> amount) | (high << (64 - amount));
    shifted.high = high >> amount;
    return shifted;
  }

  [[nodiscard]] constexpr BitBoard north() const noexcept {
    return shift_left(BOARD_WIDTH);
  }

  [[nodiscard]] constexpr BitBoard south() const noexcept {
    return shift_right(BOARD_WIDTH);
  }

  [[nodiscard]] constexpr BitBoard east() const noexcept {
    BitBoard squares = *this;
    const BitBoard right_file = file_mask(BOARD_WIDTH - 1);
    squares.low &= ~right_file.low;
    squares.high &= ~right_file.high;
    return squares.shift_left(1);
  }

  [[nodiscard]] constexpr BitBoard west() const noexcept {
    BitBoard squares = *this;
    const BitBoard left_file = file_mask(0);
    squares.low &= ~left_file.low;
    squares.high &= ~left_file.high;
    return squares.shift_right(1);
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
