#pragma once

#include "game_state.h"
#include "move.h"

GameState simulate_turn(const GameState& state, Move you_move, Move opponent_move);
