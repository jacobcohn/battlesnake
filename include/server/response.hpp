#pragma once

#include <crow.h>

#include "engine/core/position.hpp"
#include "engine/search/search.hpp"

[[nodiscard]] crow::json::wvalue make_move_response(
    const Position& position,
    const engine::search::SearchResult& result
) noexcept;
