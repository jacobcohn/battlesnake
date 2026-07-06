#pragma once

#include <cstdint>

#include "engine/core/position.hpp"

namespace engine::hash {

[[nodiscard]] uint64_t compute_space_hash(const Position& position) noexcept;
[[nodiscard]] uint64_t compute_exact_hash(const Position& position) noexcept;
void initialize_hashes(Position& position) noexcept;

}
