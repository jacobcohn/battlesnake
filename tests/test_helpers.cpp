#include "test_helpers.h"

Snake make_snake(
  const std::string& id,
  const std::vector<Coordinate>& body,
  int health,
  bool alive) {

  Snake snake;

  snake.id = id;
  snake.body = body;
  snake.health = health;
  snake.alive = alive;
  
  return snake;
}

GameState make_state(
  const Snake& you,
  const Snake& opponent,
  const std::vector<Coordinate>& food,
  int board_width,
  int board_height) {

  GameState state;

  state.you = you;
  state.opponent = opponent;
  state.food = food;
  state.board_width = board_width;
  state.board_height = board_height;

  return state;
}
