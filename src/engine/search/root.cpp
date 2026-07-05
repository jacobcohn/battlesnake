#include "internal.hpp"

#include <cstddef>

#include "engine/evaluation/evaluation.hpp"
#include "engine/move/execution.hpp"
#include "engine/move/generation.hpp"

namespace engine::search::internal {

SearchResult search_root(Position& position, int depth, SearchContext& context) noexcept {
  SearchResult result = fallback_result(position);
  result.depth = depth;

  if (should_stop(context)) {
    result.completed = false;
    return result;
  }

  if (depth <= 0 || terminal(position)) {
    result.score = engine::evaluation::evaluate(position);
    result.completed = true;
    return result;
  }

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);
  if (moves.count == 0) {
    result.score = engine::evaluation::evaluate(position);
    result.completed = true;
    return result;
  }

  int best_score = -INF;
  int alpha = -INF;
  bool found_own_move = false;

  for (Move own_move : ALL_MOVES) {
    if (should_stop(context)) {
      result.completed = false;
      return result;
    }

    int worst_score = INF;
    JointMove worst_reply{};
    bool found_reply = false;

    for (int i = 0; i < moves.count; ++i) {
      const JointMove& joint = moves.moves[static_cast<std::size_t>(i)];
      if (joint.moves[static_cast<std::size_t>(position.you)] != own_move) {
        continue;
      }

      found_own_move = true;
      found_reply = true;

      Undo undo = make_move(position, joint);
      SearchValue child = search_value(position, depth - 1, alpha, INF, context);
      unmake_move(position, undo);

      if (!child.completed) {
        result.completed = false;
        return result;
      }

      if (child.score < worst_score) {
        worst_score = child.score;
        worst_reply = joint;
      }

      if (worst_score <= alpha) {
        break;
      }
    }

    if (!found_reply) {
      continue;
    }

    if (worst_score > best_score) {
      best_score = worst_score;
      result.best_move = worst_reply;
      result.score = best_score;
    }

    if (best_score > alpha) {
      alpha = best_score;
    }
  }

  if (!found_own_move) {
    result.score = engine::evaluation::evaluate(position);
  }

  result.completed = true;
  return result;
}

}
