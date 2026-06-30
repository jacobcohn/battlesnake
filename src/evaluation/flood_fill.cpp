#include "evaluation/flood_fill.h"

#include <queue>

#include "evaluation/grid.h"

namespace {

bool can_visit(Coordinate coordinate, const GameState& state, const Grid& blocked, const Grid& visited) {
  if (is_out_of_bounds(coordinate, state.board_width, state.board_height)) {
    return false;
  }

  if (blocked[coordinate.y][coordinate.x]) {
    return false;
  }

  if (visited[coordinate.y][coordinate.x]) {
    return false;
  }

  return true;
}

int count_reachable_space(const GameState& state, Coordinate start, const Grid& blocked) {
  Grid visited(state.board_height, std::vector<bool>(state.board_width, false));
  std::queue<Coordinate> queue;

  queue.push(start);
  visited[start.y][start.x] = true;
  int reachable_count = 0;

  while (!queue.empty()) {
    Coordinate current = queue.front();
    queue.pop();
    ++reachable_count;

    for (Coordinate direction : DIRECTIONS) {
      Coordinate next{current.x + direction.x, current.y + direction.y};

      if (!can_visit(next, state, blocked, visited)) {
        continue;
      }

      visited[next.y][next.x] = true;
      queue.push(next);
    }
  }

  return reachable_count;
}

}

int count_reachable_space_from_head(const GameState& state, const Snake& snake) {
  if (!snake.alive || snake.body.empty() || state.board_width <= 0 || state.board_height <= 0) {
    return 0;
  }

  Coordinate start = snake.body.front();
  if (is_out_of_bounds(start, state.board_width, state.board_height)) {
    return 0;
  }

  Grid blocked = build_blocked_grid(state);
  blocked[start.y][start.x] = false;

  return count_reachable_space(state, start, blocked);
}
