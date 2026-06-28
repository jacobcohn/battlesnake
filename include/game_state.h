#pragma once

#include <string>
#include <vector>

struct Coordinate {
  int x = 0;
  int y = 0;
};

struct Snake {
  std::string id;
  std::vector<Coordinate> body;
  int health = 100;
  bool alive = true;
};

struct GameState {
  Snake you;
  Snake opponent;
  std::vector<Coordinate> food;
  int board_width = 0;
  int board_height = 0;
};
