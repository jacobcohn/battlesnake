#include "parse.h"

Coordinate parse_coordinate(const crow::json::rvalue& coordinate_json) {
  return {static_cast<int>(coordinate_json["x"].i()), static_cast<int>(coordinate_json["y"].i())};
}

Snake parse_snake(const crow::json::rvalue& snake_json) {
  Snake snake;
  snake.id = std::string(snake_json["id"].s());
  snake.health = static_cast<int>(snake_json["health"].i());

  for (const crow::json::rvalue& coordinate_json : snake_json["body"]) {
    snake.body.push_back(parse_coordinate(coordinate_json));
  }

  return snake;
}

GameState parse_game_state(const crow::json::rvalue& request_json) {
  GameState state;
  state.board_width = static_cast<int>(request_json["board"]["width"].i());
  state.board_height = static_cast<int>(request_json["board"]["height"].i());
  state.you = parse_snake(request_json["you"]);

  for (const crow::json::rvalue& food_json : request_json["board"]["food"]) {
    state.food.push_back(parse_coordinate(food_json));
  }

  for (const crow::json::rvalue& snake_json : request_json["board"]["snakes"]) {
    if (std::string(snake_json["id"].s()) == state.you.id) {
      continue;
    }

    state.opponent = parse_snake(snake_json);
  }

  return state;
}
