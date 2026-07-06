#include "engine/hash/zobrist.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "engine/core/constants.hpp"
#include "engine/core/move.hpp"
#include "engine/core/square.hpp"

namespace engine::hash {
namespace {

inline constexpr int DIRECTION_COUNT = 4;
inline constexpr int MAX_HEALTH = 100;

using EdgeKeys = std::array<std::array<std::array<uint64_t, DIRECTION_COUNT>, BOARD_SQUARES>, MAX_SNAKES>;
using FoodKeys = std::array<uint64_t, BOARD_SQUARES>;
using HealthKeys = std::array<std::array<uint64_t, MAX_HEALTH + 1>, MAX_SNAKES>;
using YouKeys = std::array<uint64_t, MAX_SNAKES>;
using DirectionTable = std::array<std::array<int, BOARD_SQUARES>, BOARD_SQUARES>;

[[nodiscard]] constexpr uint64_t splitmix64(uint64_t value) noexcept {
  value += 0x9e3779b97f4a7c15ULL;
  value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
  value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
  return value ^ (value >> 31);
}

[[nodiscard]] constexpr uint64_t key(uint64_t family, uint64_t a = 0, uint64_t b = 0, uint64_t c = 0) noexcept {
  uint64_t value = 0x4f1bbcdc6f2f4f85ULL;
  value ^= splitmix64(family + 0x100000001b3ULL);
  value ^= splitmix64(a + 0x9e3779b97f4a7c15ULL);
  value ^= splitmix64(b + 0xbf58476d1ce4e5b9ULL);
  value ^= splitmix64(c + 0x94d049bb133111ebULL);
  return splitmix64(value);
}

[[nodiscard]] constexpr EdgeKeys make_edge_keys() noexcept {
  EdgeKeys keys{};
  for (int snake = 0; snake < MAX_SNAKES; ++snake) {
    for (int square = 0; square < BOARD_SQUARES; ++square) {
      for (int direction = 0; direction < DIRECTION_COUNT; ++direction) {
        keys[static_cast<std::size_t>(snake)][static_cast<std::size_t>(square)][static_cast<std::size_t>(direction)] =
            key(1, static_cast<uint64_t>(snake), static_cast<uint64_t>(square), static_cast<uint64_t>(direction));
      }
    }
  }
  return keys;
}

[[nodiscard]] constexpr FoodKeys make_food_keys() noexcept {
  FoodKeys keys{};
  for (int square = 0; square < BOARD_SQUARES; ++square) {
    keys[static_cast<std::size_t>(square)] = key(2, static_cast<uint64_t>(square));
  }
  return keys;
}

[[nodiscard]] constexpr HealthKeys make_health_keys() noexcept {
  HealthKeys keys{};
  for (int snake = 0; snake < MAX_SNAKES; ++snake) {
    for (int health = 0; health <= MAX_HEALTH; ++health) {
      keys[static_cast<std::size_t>(snake)][static_cast<std::size_t>(health)] =
          key(3, static_cast<uint64_t>(snake), static_cast<uint64_t>(health));
    }
  }
  return keys;
}

[[nodiscard]] constexpr YouKeys make_you_keys() noexcept {
  YouKeys keys{};
  for (int snake = 0; snake < MAX_SNAKES; ++snake) {
    keys[static_cast<std::size_t>(snake)] = key(4, static_cast<uint64_t>(snake));
  }
  return keys;
}

[[nodiscard]] constexpr DirectionTable make_direction_table() noexcept {
  DirectionTable table{};
  for (int from = 0; from < BOARD_SQUARES; ++from) {
    for (int to = 0; to < BOARD_SQUARES; ++to) {
      table[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)] = -1;
    }
  }

  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      const Square from = idx(x, y);
      if (y + 1 < BOARD_HEIGHT) {
        table[static_cast<std::size_t>(from)][static_cast<std::size_t>(idx(x, y + 1))] = static_cast<int>(Move::Up);
      }
      if (y > 0) {
        table[static_cast<std::size_t>(from)][static_cast<std::size_t>(idx(x, y - 1))] = static_cast<int>(Move::Down);
      }
      if (x > 0) {
        table[static_cast<std::size_t>(from)][static_cast<std::size_t>(idx(x - 1, y))] = static_cast<int>(Move::Left);
      }
      if (x + 1 < BOARD_WIDTH) {
        table[static_cast<std::size_t>(from)][static_cast<std::size_t>(idx(x + 1, y))] = static_cast<int>(Move::Right);
      }
    }
  }

  return table;
}

inline constexpr EdgeKeys EDGE_KEYS = make_edge_keys();
inline constexpr FoodKeys FOOD_KEYS = make_food_keys();
inline constexpr HealthKeys HEALTH_KEYS = make_health_keys();
inline constexpr YouKeys YOU_KEYS = make_you_keys();
inline constexpr DirectionTable DIRECTIONS = make_direction_table();

[[nodiscard]] constexpr uint64_t turn_key(uint16_t turn) noexcept {
  return key(5, static_cast<uint64_t>(turn));
}

[[nodiscard]] constexpr int direction_index(Square from, Square to) noexcept {
  assert(from >= 0 && from < BOARD_SQUARES);
  assert(to >= 0 && to < BOARD_SQUARES);
  return DIRECTIONS[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)];
}

}
}
