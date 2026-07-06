#pragma once

#include <cstddef>
#include <cstdint>

#include "engine/core/position.hpp"
#include "engine/core/square.hpp"

struct Undo;

namespace engine::hash {

[[nodiscard]] uint64_t edge_hash(std::size_t snake_index, Square from, Square to) noexcept;
[[nodiscard]] uint64_t food_hash(Square square) noexcept;
[[nodiscard]] uint64_t health_hash(std::size_t snake_index, uint8_t health) noexcept;
[[nodiscard]] uint64_t turn_hash(uint16_t turn) noexcept;
[[nodiscard]] uint64_t you_hash(std::size_t snake_index) noexcept;

[[nodiscard]] uint64_t compute_snake_body_hash(const Snake& snake, std::size_t snake_index) noexcept;
[[nodiscard]] uint64_t compute_space_hash(const Position& position) noexcept;
[[nodiscard]] uint64_t compute_exact_hash(const Position& position) noexcept;
void initialize_hashes(Position& position) noexcept;
void update_after_move(Position& position, const Undo& undo) noexcept;
void restore_after_unmake(Position& position, const Undo& undo) noexcept;

}
