#include "play/simulate.h"

#include <algorithm>

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

bool is_out_of_bounds(Coordinate coordinate, int board_width, int board_height) {
  return coordinate.x < 0 || coordinate.x >= board_width || coordinate.y < 0 || coordinate.y >= board_height;
}

bool can_move(const Snake& snake) {
  return snake.alive && !snake.body.empty();
}

bool snake_will_eat_food(const Snake& snake, const std::vector<Coordinate>& food, Move move) {
  if (!can_move(snake)) {
    return false;
  }

  Coordinate next_head = apply_move(snake.body.front(), move);
  return contains_coordinate(food, next_head);
}

void move_snake(Snake& snake, Move move, bool ate_food) {
  if (!can_move(snake)) {
    return;
  }

  Coordinate next_head = apply_move(snake.body.front(), move);
  snake.health -= 1;
  snake.body.insert(snake.body.begin(), next_head);

  if (ate_food) {
    snake.health = 100;
  } else {
    snake.body.pop_back();
  }
}

void remove_eaten_food(std::vector<Coordinate>& food, const Snake& snake, bool ate_food) {
  if (!ate_food || snake.body.empty()) {
    return;
  }

  Coordinate eaten_food = snake.body.front();
  std::erase_if(food, [eaten_food](Coordinate coordinate) {
    return coordinates_equal(coordinate, eaten_food);
  });
}

bool head_collides_with_body(const Snake& snake, const Snake& body_owner, std::size_t start_index) {
  if (!can_move(snake)) {
    return false;
  }

  Coordinate head = snake.body.front();

  for (std::size_t index = start_index; index < body_owner.body.size(); ++index) {
    if (coordinates_equal(head, body_owner.body[index])) {
      return true;
    }
  }

  return false;
}

bool died_from_wall(const Snake& snake, int board_width, int board_height) {
  if (!can_move(snake)) {
    return false;
  }

  return is_out_of_bounds(snake.body.front(), board_width, board_height);
}

bool died_from_starvation(const Snake& snake) {
  if (!can_move(snake)) {
    return false;
  }

  return snake.health <= 0;
}

bool died_from_body_collision(const Snake& snake, const Snake& opponent) {
  return head_collides_with_body(snake, snake, 1) || head_collides_with_body(snake, opponent, 1);
}

bool heads_collide(const GameState& state) {
  if (!can_move(state.you) || !can_move(state.opponent)) {
    return false;
  }

  return coordinates_equal(state.you.body.front(), state.opponent.body.front());
}

void mark_head_to_head_deaths(const GameState& state, bool& you_died, bool& opponent_died) {
  if (you_died || opponent_died || !heads_collide(state)) {
    return;
  }

  if (state.you.body.size() > state.opponent.body.size()) {
    opponent_died = true;
  } else if (state.you.body.size() < state.opponent.body.size()) {
    you_died = true;
  } else {
    you_died = true;
    opponent_died = true;
  }
}

bool died_before_head_to_head(const Snake& snake, const Snake& opponent, int board_width, int board_height) {
  if (died_from_wall(snake, board_width, board_height)) {
    return true;
  }

  if (died_from_starvation(snake)) {
    return true;
  }

  return died_from_body_collision(snake, opponent);
}

void mark_deaths(GameState& state) {
  bool you_died = died_before_head_to_head(state.you, state.opponent, state.board_width, state.board_height);
  bool opponent_died = died_before_head_to_head(state.opponent, state.you, state.board_width, state.board_height);

  mark_head_to_head_deaths(state, you_died, opponent_died);

  if (you_died) {
    state.you.alive = false;
  }

  if (opponent_died) {
    state.opponent.alive = false;
  }
}

}

GameState simulate_turn(const GameState& state, Move you_move, Move opponent_move) {
  GameState next_state = state;

  bool you_ate_food = snake_will_eat_food(state.you, state.food, you_move);
  bool opponent_ate_food = snake_will_eat_food(state.opponent, state.food, opponent_move);

  move_snake(next_state.you, you_move, you_ate_food);
  move_snake(next_state.opponent, opponent_move, opponent_ate_food);

  remove_eaten_food(next_state.food, next_state.you, you_ate_food);
  remove_eaten_food(next_state.food, next_state.opponent, opponent_ate_food);

  mark_deaths(next_state);

  return next_state;
}
