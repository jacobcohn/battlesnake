#pragma once

#include <crow.h>

#include "engine/core/position.hpp"

[[nodiscard]] Position parse_position(const crow::json::rvalue& body);
