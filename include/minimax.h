#pragma once

#include "game_state.h"
#include "move.h"

Move choose_minimax_move(const GameState& state, int depth);
