#pragma once

#include "evaluation/params.h"
#include "game_state.h"

int evaluate_state(const GameState& state, const EvaluationParams& params);
int evaluate_state(const GameState& state);
