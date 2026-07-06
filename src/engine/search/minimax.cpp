#include "internal.hpp"

#include <cstddef>

#include "engine/evaluation/evaluation.hpp"
#include "engine/move/execution.hpp"
#include "engine/move/generation.hpp"

namespace engine::search::internal {

SearchValue search_value(Position& position, int depth, int alpha, int beta, SearchContext& context) noexcept {
  if (should_stop(context)) {
    return SearchValue{0, false};
  }

  ++context.nodes;

  if (depth <= 0 || terminal(position)) {
    return SearchValue{engine::evaluation::evaluate(position), true};
  }

  const engine::move::JointMoveList& moves = engine::move::generate_joint_moves(position);
  if (moves.count == 0) {
    return SearchValue{engine::evaluation::evaluate(position), true};
  }

  int best_score = -INF;
  int best_guaranteed_score = alpha;
  bool found_own_move = false;

  for (Move own_move : ALL_MOVES) {
    if (should_stop(context)) {
      return SearchValue{0, false};
    }

    int worst_score = INF;
    bool found_reply = false;

    for (int i = 0; i < moves.count; ++i) {
      const JointMove& joint = moves.moves[static_cast<std::size_t>(i)];
      if (joint.moves[static_cast<std::size_t>(position.you)] != own_move) {
        continue;
      }

      found_own_move = true;
      found_reply = true;

      Undo undo = make_move(position, joint);
      SearchValue child = search_value(position, depth - 1, best_guaranteed_score, beta, context);
      unmake_move(position, undo);

      if (!child.completed) {
        return SearchValue{0, false};
      }

      if (child.score < worst_score) {
        worst_score = child.score;
      }

      if (worst_score <= best_guaranteed_score) {
        ++context.cutoffs;
        break;
      }
    }

    if (!found_reply) {
      continue;
    }

    if (worst_score > best_score) {
      best_score = worst_score;
    }

    if (best_score > best_guaranteed_score) {
      best_guaranteed_score = best_score;
    }

    if (best_guaranteed_score >= beta) {
      ++context.cutoffs;
      break;
    }
  }

  if (!found_own_move) {
    return SearchValue{engine::evaluation::evaluate(position), true};
  }

  return SearchValue{best_score, true};
}

}
