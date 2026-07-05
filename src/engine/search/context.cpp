#include "internal.hpp"

namespace engine::search::internal {

SearchContext make_context(const SearchLimits& limits) noexcept {
  SearchContext context;
  if (limits.time_limit_ms > 0) {
    context.has_deadline = true;
    context.deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(limits.time_limit_ms);
  }

  return context;
}

bool should_stop(SearchContext& context) noexcept {
  if (!context.has_deadline) {
    return false;
  }

  if (std::chrono::steady_clock::now() < context.deadline) {
    return false;
  }

  context.stopped = true;
  return true;
}

}
