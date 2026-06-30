#include "evaluation/territory.h"

#include <limits>
#include <queue>
#include <vector>

#include "evaluation/grid.h"

namespace {

using DistanceGrid = std::vector<std::vector<int>>;

constexpr int UNREACHABLE = std::numeric_limits<int>::max();

bool can_visit(Coordinate coordinate, int board_width, int board_height, const Grid& blocked, const DistanceGrid& distances) {
  if (is_out_of_bounds(coordinate, board_width, board_height)) {
    return false;
  }

  if (blocked[coordinate.y][coordinate.x]) {
    return false;
  }

  if (distances[coordinate.y][coordinate.x] != UNREACHABLE) {
    return false;
  }

  return true;
}

DistanceGrid calculate_distances(const GameState& state, Coordinate start, Grid blocked) {
  DistanceGrid distances(state.board_height, std::vector<int>(state.board_width, UNREACHABLE));

  if (is_out_of_bounds(start, state.board_width, state.board_height)) {
    return distances;
  }

  blocked[start.y][start.x] = false;

  std::queue<Coordinate> queue;
  queue.push(start);
  distances[start.y][start.x] = 0;

  while (!queue.empty()) {
    Coordinate current = queue.front();
    queue.pop();

    for (Coordinate direction : DIRECTIONS) {
      Coordinate next{current.x + direction.x, current.y + direction.y};

      if (!can_visit(next, state.board_width, state.board_height, blocked, distances)) {
        continue;
      }

      distances[next.y][next.x] = distances[current.y][current.x] + 1;
      queue.push(next);
    }
  }

  return distances;
}

int score_square_control(int you_distance, int opponent_distance) {
  if (you_distance == UNREACHABLE && opponent_distance == UNREACHABLE) {
    return 0;
  }

  if (you_distance != UNREACHABLE && opponent_distance == UNREACHABLE) {
    return 1;
  }

  if (you_distance == UNREACHABLE && opponent_distance != UNREACHABLE) {
    return -1;
  }

  if (you_distance < opponent_distance) {
    return 1;
  }

  if (you_distance > opponent_distance) {
    return -1;
  }

  return 0;
}

}

int evaluate_territory_control(const GameState& state) {
  if (!state.you.alive || !state.opponent.alive || state.you.body.empty() || state.opponent.body.empty()) {
    return 0;
  }

  if (state.board_width <= 0 || state.board_height <= 0) {
    return 0;
  }

  Grid blocked = build_blocked_grid(state);
  DistanceGrid you_distances = calculate_distances(state, state.you.body.front(), blocked);
  DistanceGrid opponent_distances = calculate_distances(state, state.opponent.body.front(), blocked);

  int territory_score = 0;

  for (int y = 0; y < state.board_height; ++y) {
    for (int x = 0; x < state.board_width; ++x) {
      territory_score += score_square_control(you_distances[y][x], opponent_distances[y][x]);
    }
  }

  return territory_score;
}
