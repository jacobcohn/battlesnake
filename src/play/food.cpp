#include "play/food.h"

#include <cstddef>
#include <random>
#include <vector>

namespace {

bool coordinates_equal(Coordinate left, Coordinate right) {
  return left.x == right.x && left.y == right.y;
}

bool contains_coordinate(const std::vector<Coordinate>& coordinates, Coordinate target) {
  for (Coordinate coordinate : coordinates) {
    if (coordinates_equal(coordinate, target)) {
      return true;
    }
  }

  return false;
}

bool coordinate_is_occupied(const GameState& state, Coordinate coordinate) {
  if (contains_coordinate(state.you.body, coordinate)) {
    return true;
  }

  if (contains_coordinate(state.opponent.body, coordinate)) {
    return true;
  }

  if (contains_coordinate(state.food, coordinate)) {
    return true;
  }

  return false;
}

std::vector<Coordinate> find_empty_cells(const GameState& state) {
  std::vector<Coordinate> empty_cells;

  for (int y = 0; y < state.board_height; ++y) {
    for (int x = 0; x < state.board_width; ++x) {
      Coordinate coordinate{x, y};
      if (!coordinate_is_occupied(state, coordinate)) {
        empty_cells.push_back(coordinate);
      }
    }
  }

  return empty_cells;
}

std::mt19937& rng() {
  static std::random_device device;
  static std::mt19937 generator(device());
  return generator;
}

void spawn_one_food(GameState& state) {
  std::vector<Coordinate> empty_cells = find_empty_cells(state);
  if (empty_cells.empty()) {
    return;
  }

  std::uniform_int_distribution<std::size_t> distribution(0, empty_cells.size() - 1);
  state.food.push_back(empty_cells[distribution(rng())]);
}

bool should_spawn_food(int food_spawn_chance) {
  std::uniform_int_distribution<int> distribution(1, 100);
  return distribution(rng()) <= food_spawn_chance;
}

}

void apply_food_rules(GameState& state, const FoodRules& rules) {
  while (static_cast<int>(state.food.size()) < rules.minimum_food) {
    std::size_t before = state.food.size();
    spawn_one_food(state);

    if (state.food.size() == before) {
      return;
    }
  }

  if (should_spawn_food(rules.food_spawn_chance)) {
    spawn_one_food(state);
  }
}
