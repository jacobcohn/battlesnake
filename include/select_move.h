#pragma once

#include <string>

#include "game_state.h"

std::string select_move(const GameState& state, int search_depth = 6);
