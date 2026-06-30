#pragma once

#include <vector>

#include "game_state.h"

using Grid = std::vector<std::vector<bool>>;

constexpr Coordinate DIRECTIONS[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

bool is_out_of_bounds(Coordinate coordinate, int board_width, int board_height);
void mark_snake_body_as_blocked(Grid& blocked, const Snake& snake, int board_width, int board_height);
Grid build_blocked_grid(const GameState& state);
