#pragma once

#include <crow.h>

#include "game_state.h"

Coordinate parse_coordinate(const crow::json::rvalue& coordinate_json);
Snake parse_snake(const crow::json::rvalue& snake_json);
GameState parse_game_state(const crow::json::rvalue& request_json);
