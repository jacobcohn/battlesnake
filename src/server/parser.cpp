#include "server/parser.hpp"

#include <stdexcept>
#include <string>
#include <vector>

#include "engine/bitboard.hpp"
#include "engine/constants.hpp"
#include "engine/square.hpp"

namespace {

void require_type(const crow::json::rvalue& value, crow::json::type type, const char* field) {
  if (value.t() != type) {
    throw std::runtime_error(std::string{"invalid or missing field: "} + field);
  }
}

const crow::json::rvalue& require_object_field(const crow::json::rvalue& object, const char* field) {
  require_type(object, crow::json::type::Object, field);
  if (!object.has(field)) {
    throw std::runtime_error(std::string{"missing field: "} + field);
  }

  const crow::json::rvalue& value = object[field];
  require_type(value, crow::json::type::Object, field);
  return value;
}

const crow::json::rvalue& require_list_field(const crow::json::rvalue& object, const char* field) {
  require_type(object, crow::json::type::Object, field);
  if (!object.has(field)) {
    throw std::runtime_error(std::string{"missing field: "} + field);
  }

  const crow::json::rvalue& value = object[field];
  require_type(value, crow::json::type::List, field);
  return value;
}

int require_int_field(const crow::json::rvalue& object, const char* field) {
  require_type(object, crow::json::type::Object, field);
  if (!object.has(field)) {
    throw std::runtime_error(std::string{"missing field: "} + field);
  }

  const crow::json::rvalue& value = object[field];
  require_type(value, crow::json::type::Number, field);
  return static_cast<int>(value.i());
}

std::string require_string_field(const crow::json::rvalue& object, const char* field) {
  require_type(object, crow::json::type::Object, field);
  if (!object.has(field)) {
    throw std::runtime_error(std::string{"missing field: "} + field);
  }

  const crow::json::rvalue& value = object[field];
  require_type(value, crow::json::type::String, field);
  return static_cast<std::string>(value.s());
}

Square parse_coord(const crow::json::rvalue& coord) {
  const int x = require_int_field(coord, "x");
  const int y = require_int_field(coord, "y");

  if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
    throw std::runtime_error("coordinate out of bounds");
  }

  return idx(x, y);
}

std::string parse_snake(const crow::json::rvalue& value, Snake& snake) {
  require_type(value, crow::json::type::Object, "snake");

  snake.reset();

  const std::string id = require_string_field(value, "id");
  const int health = require_int_field(value, "health");
  const int length = require_int_field(value, "length");
  const crow::json::rvalue& body = require_list_field(value, "body");

  if (body.size() == 0) {
    throw std::runtime_error("snake body is empty");
  }

  if (length <= 0 || length > BOARD_SQUARES) {
    throw std::runtime_error("invalid snake length");
  }

  if (body.size() != static_cast<std::size_t>(length)) {
    throw std::runtime_error("snake length does not match body size");
  }

  if (health < 0 || health > 100) {
    throw std::runtime_error("invalid snake health");
  }

  std::vector<Square> squares;
  squares.reserve(body.size());

  BitBoard seen;
  for (std::size_t i = 0; i < body.size(); ++i) {
    const Square square = parse_coord(body[i]);

    if (seen.test(square)) {
      throw std::runtime_error("duplicate square in snake body");
    }

    seen.set(square);
    squares.push_back(square);
    snake.occupied.set(square);
  }

  snake.head = squares.front();
  snake.tail = squares.back();
  snake.length = static_cast<uint8_t>(length);
  snake.health = static_cast<uint8_t>(health);
  snake.alive = true;

  for (std::size_t i = squares.size() - 1; i > 0; --i) {
    snake.next[static_cast<std::size_t>(squares[i])] = squares[i - 1];
  }

  return id;
}

}

Position parse_position(const crow::json::rvalue& body) {
  require_type(body, crow::json::type::Object, "body");

  Position position;
  position.reset();

  const int turn = require_int_field(body, "turn");
  if (turn < 0) {
    throw std::runtime_error("invalid turn");
  }

  position.turn = static_cast<uint16_t>(turn);

  const crow::json::rvalue& board = require_object_field(body, "board");
  const crow::json::rvalue& you = require_object_field(body, "you");

  const int width = require_int_field(board, "width");
  const int height = require_int_field(board, "height");

  if (width != BOARD_WIDTH || height != BOARD_HEIGHT) {
    throw std::runtime_error("invalid board dimensions");
  }

  const crow::json::rvalue& food = require_list_field(board, "food");
  for (std::size_t i = 0; i < food.size(); ++i) {
    position.food.set(parse_coord(food[i]));
  }

  const std::string you_id = require_string_field(you, "id");

  const crow::json::rvalue& snakes = require_list_field(board, "snakes");
  if (snakes.size() == 0) {
    throw std::runtime_error("no snakes");
  }

  if (snakes.size() > MAX_SNAKES) {
    throw std::runtime_error("too many snakes");
  }

  bool found_you = false;
  position.snake_count = static_cast<uint8_t>(snakes.size());

  for (std::size_t i = 0; i < snakes.size(); ++i) {
    const std::string snake_id = parse_snake(snakes[i], position.snakes[i]);

    if (snake_id == you_id) {
      position.you = static_cast<uint8_t>(i);
      found_you = true;
    }
  }

  if (!found_you) {
    throw std::runtime_error("missing own snake id");
  }

  position.build_occupied();
  return position;
}
