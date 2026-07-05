#pragma once

#include "engine/move/execution.hpp"
#include "engine/move/targets.hpp"

void prepare_undo(const Position& position, Undo& undo) noexcept;
void compute_move_targets(const Position& position, const JointMove& joint, Undo& undo) noexcept;

void move_heads(Position& position, Undo& undo) noexcept;
void remove_non_growing_tails(Position& position, Undo& undo) noexcept;
void reduce_health(Position& position, const Undo& undo) noexcept;
void apply_food(Position& position, const Undo& undo) noexcept;

void mark_health_deaths(Position& position, Undo& undo) noexcept;
void mark_body_collisions(const Position& position, Undo& undo) noexcept;
void mark_head_to_head_deaths(const Position& position, Undo& undo) noexcept;
void remove_dead_snakes(Position& position, const Undo& undo) noexcept;
void build_alive_occupied(Position& position) noexcept;

void restore_snakes(Position& position, const Undo& undo) noexcept;
