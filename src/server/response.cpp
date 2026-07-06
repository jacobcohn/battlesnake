#include "server/response.hpp"

#include <cstddef>
#include <cstdint>

#include "engine/core/move.hpp"

crow::json::wvalue make_move_response(
    const Position& position,
    const engine::search::SearchResult& result
) noexcept {
  const char* selected_move = move_name(result.best_move.moves[static_cast<std::size_t>(position.you)]);

  uint64_t nodes = 0;
  uint64_t cutoffs = 0;
  uint64_t elapsed_ms = 0;
  for (int i = 0; i < result.depth_count; ++i) {
    const engine::search::DepthInfo& depth = result.depths[static_cast<std::size_t>(i)];
    nodes += depth.nodes;
    cutoffs += depth.cutoffs;
    elapsed_ms += depth.elapsed_ms;
  }

  crow::json::wvalue response_body;
  response_body["move"] = selected_move;
  response_body["score"] = result.score;
  response_body["depth"] = result.depth;
  response_body["completed"] = result.completed;
  response_body["timed_out"] = result.timed_out;
  response_body["nodes"] = nodes;
  response_body["cutoffs"] = cutoffs;
  response_body["elapsed_ms"] = elapsed_ms;
  return response_body;
}
