#include "internal.hpp"

#include "engine/evaluation/evaluation.hpp"
#include "engine/move/generation.hpp"

namespace engine::search::internal {

SearchResult fallback_result(const Position& position) noexcept {
  SearchResult result;
  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);
  if (moves.count > 0) {
    result.best_move = moves.moves[0];
  }

  result.score = engine::evaluation::evaluate(position);
  result.depth = 0;
  result.completed = false;
  return result;
}

}
