#pragma once

#include <crow.h>

#include "engine/position.hpp"

[[nodiscard]] Position parse_position(const crow::json::rvalue& body);
