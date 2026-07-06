#include "engine/search/transposition_table.hpp"

#include <new>

namespace engine::search {

namespace {

[[nodiscard]] bool is_power_of_two(std::size_t value) noexcept {
  return value != 0 && (value & (value - 1)) == 0;
}

[[nodiscard]] std::size_t index(const TranspositionTable& table, uint64_t key) noexcept {
  return static_cast<std::size_t>(key) & (table.entry_count - 1);
}

}

void initialize(TranspositionTable& table, std::size_t entry_count) noexcept {
  table.entries.reset();
  table.entry_count = 0;

  if (!is_power_of_two(entry_count)) {
    return;
  }

  table.entries.reset(new (std::nothrow) TranspositionEntry[entry_count]);
  if (!table.entries) {
    return;
  }

  table.entry_count = entry_count;
}

void clear(TranspositionTable& table) noexcept {
  if (!enabled(table)) {
    return;
  }

  for (std::size_t i = 0; i < table.entry_count; ++i) {
    table.entries[i] = TranspositionEntry{};
  }
}

bool enabled(const TranspositionTable& table) noexcept {
  return table.entries != nullptr && table.entry_count != 0;
}

const TranspositionEntry* find(const TranspositionTable& table, uint64_t key) noexcept {
  if (!enabled(table)) {
    return nullptr;
  }

  const TranspositionEntry& entry = table.entries[index(table, key)];
  if (!entry.occupied || entry.key != key) {
    return nullptr;
  }

  return &entry;
}

bool store(
    TranspositionTable& table,
    uint64_t key,
    int depth,
    int score,
    BoundType bound,
    const JointMove& best_move,
    bool has_best_move) noexcept {
  if (!enabled(table)) {
    return false;
  }

  TranspositionEntry& entry = table.entries[index(table, key)];
  if (entry.occupied && entry.key != key && depth < entry.depth) {
    return false;
  }

  entry.key = key;
  entry.depth = depth;
  entry.score = score;
  entry.bound = bound;
  entry.best_move = best_move;
  entry.has_best_move = has_best_move;
  entry.occupied = true;
  return true;
}

}
