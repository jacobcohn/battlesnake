#pragma once

#include "engine/core/position.hpp"
#include "engine/search/search.hpp"

void log_search_result(const Position& position, const engine::search::SearchResult& result) noexcept;
