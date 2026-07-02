#include <stdexcept>
#include <string>

#include <crow.h>
#include <doctest/doctest.h>

#include "engine/constants.hpp"
#include "engine/position.hpp"
#include "engine/square.hpp"
#include "server/parser.hpp"

namespace {

Position parse_json(const std::string& json) {
  const crow::json::rvalue body = crow::json::load(json);
  REQUIRE(body);
  return parse_position(body);
}

void expect_parse_failure(const std::string& json) {
  const crow::json::rvalue body = crow::json::load(json);
  REQUIRE(body);
  CHECK_THROWS_AS(static_cast<void>(parse_position(body)), std::runtime_error);
}

std::string valid_payload() {
  return R"json(
{
  "game": { "id": "game-id", "ruleset": { "name": "standard", "version": "v1.2.3" }, "timeout": 500 },
  "turn": 7,
  "board": {
    "height": 11,
    "width": 11,
    "food": [ { "x": 2, "y": 3 }, { "x": 10, "y": 10 } ],
    "hazards": [ { "x": 0, "y": 10 }, { "x": 9, "y": 9 } ],
    "snakes": [
      {
        "id": "you-id",
        "name": "us",
        "health": 87,
        "body": [ { "x": 5, "y": 5 }, { "x": 5, "y": 4 }, { "x": 5, "y": 3 } ],
        "latency": "0",
        "head": { "x": 5, "y": 5 },
        "length": 3,
        "shout": ""
      },
      {
        "id": "enemy-id",
        "name": "them",
        "health": 42,
        "body": [ { "x": 8, "y": 8 }, { "x": 8, "y": 7 }, { "x": 8, "y": 6 } ],
        "latency": "0",
        "head": { "x": 8, "y": 8 },
        "length": 3,
        "shout": ""
      }
    ]
  },
  "you": {
    "id": "you-id",
    "name": "us",
    "health": 87,
    "body": [ { "x": 5, "y": 5 }, { "x": 5, "y": 4 }, { "x": 5, "y": 3 } ],
    "latency": "0",
    "head": { "x": 5, "y": 5 },
    "length": 3,
    "shout": ""
  }
}
)json";
}

}

TEST_CASE("parse_position parses valid request metadata") {
  const Position position = parse_json(valid_payload());

  CHECK(position.turn == 7);
  CHECK(position.snake_count == 2);
  CHECK(position.you == 0);
}

TEST_CASE("parse_position parses food and hazards") {
  const Position position = parse_json(valid_payload());

  CHECK(position.food.count() == 2);
  CHECK(position.food.test(idx(2, 3)));
  CHECK(position.food.test(idx(10, 10)));
  CHECK_FALSE(position.food.test(idx(0, 0)));

  CHECK(position.hazards.count() == 2);
  CHECK(position.hazards.test(idx(0, 10)));
  CHECK(position.hazards.test(idx(9, 9)));
  CHECK_FALSE(position.hazards.test(idx(1, 1)));
}

TEST_CASE("parse_position parses snake body linked from tail to head") {
  const Position position = parse_json(valid_payload());
  const Snake& snake = position.snakes[position.you];

  const Square head = idx(5, 5);
  const Square middle = idx(5, 4);
  const Square tail = idx(5, 3);

  CHECK(snake.head == head);
  CHECK(snake.tail == tail);
  CHECK(snake.length == 3);
  CHECK(snake.health == 87);
  CHECK(snake.alive);
  CHECK(snake.next[static_cast<std::size_t>(tail)] == middle);
  CHECK(snake.next[static_cast<std::size_t>(middle)] == head);
  CHECK(snake.next[static_cast<std::size_t>(head)] == INVALID_SQUARE);
}

TEST_CASE("parse_position rebuilds occupied squares") {
  const Position position = parse_json(valid_payload());

  CHECK(position.occupied.count() == 6);
  CHECK(position.occupied.test(idx(5, 5)));
  CHECK(position.occupied.test(idx(5, 4)));
  CHECK(position.occupied.test(idx(5, 3)));
  CHECK(position.occupied.test(idx(8, 8)));
  CHECK(position.occupied.test(idx(8, 7)));
  CHECK(position.occupied.test(idx(8, 6)));
  CHECK_FALSE(position.occupied.test(idx(0, 0)));
}

TEST_CASE("parse_position rejects invalid board dimensions") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 7, "width": 7, "food": [], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": 1, "y": 1 } ], "length": 1 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}

TEST_CASE("parse_position rejects too many snakes") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": 1, "y": 1 } ], "length": 1 },
    { "id": "enemy-a", "health": 100, "body": [ { "x": 2, "y": 2 } ], "length": 1 },
    { "id": "enemy-b", "health": 100, "body": [ { "x": 3, "y": 3 } ], "length": 1 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}

TEST_CASE("parse_position rejects missing own snake") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [], "hazards": [], "snakes": [
    { "id": "enemy-id", "health": 100, "body": [ { "x": 1, "y": 1 } ], "length": 1 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}

TEST_CASE("parse_position rejects duplicate squares in snake body") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": 1, "y": 1 }, { "x": 1, "y": 1 } ], "length": 2 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}

TEST_CASE("parse_position rejects snake length mismatch") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": 1, "y": 1 }, { "x": 1, "y": 2 } ], "length": 3 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}

TEST_CASE("parse_position rejects out-of-bounds coordinates") {
  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [ { "x": 11, "y": 0 } ], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": 1, "y": 1 } ], "length": 1 }
  ] },
  "you": { "id": "you-id" }
}
)json");

  expect_parse_failure(R"json(
{
  "turn": 0,
  "board": { "height": 11, "width": 11, "food": [], "hazards": [], "snakes": [
    { "id": "you-id", "health": 100, "body": [ { "x": -1, "y": 0 } ], "length": 1 }
  ] },
  "you": { "id": "you-id" }
}
)json");
}
