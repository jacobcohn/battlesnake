#pragma once

#include <string>
#include <vector>

#include "game_state.h"

Snake make_snake(
  const std::string& id,
  const std::vector<Coordinate>& body,
  int health = 100,
  bool alive = true);

GameState make_state(
  const Snake& you,
  const Snake& opponent,
  const std::vector<Coordinate>& food = {},
  int board_width = 11,
  int board_height = 11);
