#pragma once

#include "evaluation/params.h"
#include "play/game.h"
#include "play/rules.h"

Game play_game(
  int width,
  int height,
  const EvaluationParams& snake_a,
  const EvaluationParams& snake_b,
  const FoodRules& food_rules);
