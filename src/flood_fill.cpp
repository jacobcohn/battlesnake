#include "flood_fill.h"

#include <queue>
#include <vector>

namespace {

using Grid = std::vector<std::vector<bool>>;

constexpr Coordinate DIRECTIONS[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

bool is_out_of_bounds(Coordinate coordinate, int board_width, int board_height) {
  if (coordinate.x < 0) {
    return true;
  }

  if (coordinate.x >= board_width) {
    return true;
  }

  if (coordinate.y < 0) {
    return true;
  }

  if (coordinate.y >= board_height) {
    return true;
  }

  return false;
}

void mark_snake_body_as_blocked(Grid& blocked, const Snake& snake, int board_width, int board_height) {
  for (Coordinate coordinate : snake.body) {
    if (!is_out_of_bounds(coordinate, board_width, board_height)) {
      blocked[coordinate.y][coordinate.x] = true;
    }
  }
}

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

  Grid blocked(state.board_height, std::vector<bool>(state.board_width, false));
  mark_snake_body_as_blocked(blocked, state.you, state.board_width, state.board_height);
  mark_snake_body_as_blocked(blocked, state.opponent, state.board_width, state.board_height);
  blocked[start.y][start.x] = false;

  return count_reachable_space(state, start, blocked);
}
