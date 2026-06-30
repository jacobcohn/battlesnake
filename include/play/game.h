#pragma once

#include <vector>

#include "evaluation/params.h"
#include "game_state.h"

enum class GameOutcome {
  Playing,
  SnakeAWin,
  SnakeBWin,
  Draw,
};

struct Game {
  EvaluationParams snake_a;
  EvaluationParams snake_b;
  std::vector<GameState> states;
  GameOutcome outcome = GameOutcome::Playing;
};
