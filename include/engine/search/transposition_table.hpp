#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "engine/core/move.hpp"

namespace engine::search {

enum class BoundType : uint8_t {
  Exact,
  LowerBound,
  UpperBound,
};

struct TranspositionEntry {
  uint64_t key = 0;
  int depth = 0;
  int score = 0;
  BoundType bound = BoundType::Exact;
  JointMove best_move{};
  bool has_best_move = false;
  bool occupied = false;
};

struct TranspositionTable {
  std::unique_ptr<TranspositionEntry[]> entries{};
  std::size_t entry_count = 0;
};

void initialize(TranspositionTable& table, std::size_t entry_count) noexcept;
void clear(TranspositionTable& table) noexcept;

[[nodiscard]] bool enabled(const TranspositionTable& table) noexcept;
[[nodiscard]] const TranspositionEntry* find(const TranspositionTable& table, uint64_t key) noexcept;

[[nodiscard]] bool store(
    TranspositionTable& table,
    uint64_t key,
    int depth,
    int score,
    BoundType bound,
    const JointMove& best_move,
    bool has_best_move) noexcept;

}
