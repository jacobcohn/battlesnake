#include "engine/search/search.hpp"

#include "internal.hpp"

namespace engine::search {

SearchResult search(Position& position, const SearchLimits& limits) noexcept {
  SearchResult best_completed = internal::fallback_result(position);
  const int max_depth = limits.max_depth > 0 ? limits.max_depth : 0;

  if (max_depth == 0) {
    best_completed.completed = true;
    return best_completed;
  }

  internal::SearchContext context = internal::make_context(limits);

  for (int depth = 1; depth <= max_depth; ++depth) {
    SearchResult depth_result = internal::search_root(position, depth, context);
    if (!depth_result.completed) {
      best_completed.completed = false;
      return best_completed;
    }

    best_completed = depth_result;
    best_completed.depth = depth;
    best_completed.completed = depth == max_depth;
  }

  best_completed.completed = true;
  return best_completed;
}

}
