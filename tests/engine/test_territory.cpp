#include <array>
#include <cstddef>
#include <initializer_list>

#include <doctest/doctest.h>

#include "engine/core/constants.hpp"
#include "engine/core/position.hpp"
#include "engine/core/square.hpp"
#include "engine/evaluation/territory/availability.hpp"
#include "engine/evaluation/territory/claims.hpp"
#include "engine/evaluation/territory/frontiers.hpp"
#include "engine/evaluation/territory/territory.hpp"

namespace {

void make_snake(Snake& snake, std::initializer_list<Square> body, bool alive = true) {
  snake.reset();

  snake.length = static_cast<unsigned char>(body.size());
  snake.health = 100;
  snake.alive = alive;

  Square previous = INVALID_SQUARE;
  bool first = true;

  for (const Square square : body) {
    if (first) {
      snake.tail = square;
      first = false;
    } else {
      snake.next[static_cast<std::size_t>(previous)] = square;
    }

    snake.head = square;
    snake.occupied.set(square);
    previous = square;
  }
}

Position make_position() {
  Position position;
  position.reset();
  return position;
}

engine::evaluation::TerritoryControl reference_territory_control(const Position& position) {
  constexpr int horizon = BOARD_SQUARES * 2;
  constexpr int unreached_turn = horizon + 1;
  constexpr int no_snake = -1;

  std::array<int, BOARD_SQUARES> available_turn{};
  std::array<int, BOARD_SQUARES> claim_snake{};
  std::array<int, BOARD_SQUARES> claim_turn{};
  std::array<int, BOARD_SQUARES> claim_length{};
  std::array<bool, BOARD_SQUARES> contested{};
  std::array<std::array<bool, BOARD_SQUARES>, MAX_SNAKES> current{};
  std::array<std::array<bool, BOARD_SQUARES>, MAX_SNAKES> next{};
  std::array<std::array<bool, BOARD_SQUARES>, MAX_SNAKES> reached{};

  claim_snake.fill(no_snake);
  claim_turn.fill(unreached_turn);
  claim_length.fill(0);

  for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
    const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
    if (!snake.alive || snake.length == 0) {
      continue;
    }

    Square square = snake.tail;
    for (int release_turn = 1; square != INVALID_SQUARE; ++release_turn) {
      if (release_turn > available_turn[static_cast<std::size_t>(square)]) {
        available_turn[static_cast<std::size_t>(square)] = release_turn;
      }

      if (square == snake.head) {
        break;
      }
      square = snake.next[static_cast<std::size_t>(square)];
    }

    current[static_cast<std::size_t>(snake_index)][static_cast<std::size_t>(snake.head)] = true;
  }

  for (int turn = 1; turn <= horizon; ++turn) {
    for (std::array<bool, BOARD_SQUARES>& snake_next : next) {
      snake_next.fill(false);
    }

    bool has_next = false;
    for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
      const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
      if (!snake.alive || snake.length == 0) {
        continue;
      }

      for (Square square = 0; square < BOARD_SQUARES; ++square) {
        if (!current[static_cast<std::size_t>(snake_index)][static_cast<std::size_t>(square)]) {
          continue;
        }

        const int x = x_of(square);
        const int y = y_of(square);
        const Square neighbors[] = {
            x > 0 ? idx(x - 1, y) : INVALID_SQUARE,
            x + 1 < BOARD_WIDTH ? idx(x + 1, y) : INVALID_SQUARE,
            y > 0 ? idx(x, y - 1) : INVALID_SQUARE,
            y + 1 < BOARD_HEIGHT ? idx(x, y + 1) : INVALID_SQUARE,
        };

        for (const Square neighbor : neighbors) {
          if (neighbor == INVALID_SQUARE) {
            continue;
          }
          if (available_turn[static_cast<std::size_t>(neighbor)] > turn) {
            continue;
          }

          next[static_cast<std::size_t>(snake_index)][static_cast<std::size_t>(neighbor)] = true;
          has_next = true;
        }
      }
    }

    if (!has_next) {
      break;
    }

    for (int snake_index = 0; snake_index < position.snake_count; ++snake_index) {
      const Snake& snake = position.snakes[static_cast<std::size_t>(snake_index)];
      if (!snake.alive || snake.length == 0) {
        continue;
      }

      for (Square square = 0; square < BOARD_SQUARES; ++square) {
        const std::size_t index = static_cast<std::size_t>(square);
        if (!next[static_cast<std::size_t>(snake_index)][index]) {
          continue;
        }
        if (reached[static_cast<std::size_t>(snake_index)][index]) {
          continue;
        }

        reached[static_cast<std::size_t>(snake_index)][index] = true;

        if (turn < claim_turn[index]) {
          claim_snake[index] = snake_index;
          claim_turn[index] = turn;
          claim_length[index] = snake.length;
          contested[index] = false;
        } else if (turn == claim_turn[index]) {
          if (snake.length > claim_length[index]) {
            claim_snake[index] = snake_index;
            claim_length[index] = snake.length;
            contested[index] = false;
          } else if (snake.length == claim_length[index]) {
            claim_snake[index] = no_snake;
            contested[index] = true;
          }
        }
      }
    }

    bool all_resolved = true;
    for (Square square = 0; square < BOARD_SQUARES; ++square) {
      if (claim_turn[static_cast<std::size_t>(square)] == unreached_turn) {
        all_resolved = false;
        break;
      }
    }
    if (all_resolved) {
      break;
    }

    current = next;
  }

  engine::evaluation::TerritoryControl control{};
  for (Square square = 0; square < BOARD_SQUARES; ++square) {
    const std::size_t index = static_cast<std::size_t>(square);
    if (claim_snake[index] == no_snake || contested[index]) {
      continue;
    }
    ++control[static_cast<std::size_t>(claim_snake[index])];
  }

  return control;
}

void check_matches_reference(const Position& position) {
  const engine::evaluation::TerritoryControl actual =
      engine::evaluation::compute_territory_control(position);
  const engine::evaluation::TerritoryControl expected = reference_territory_control(position);

  CHECK(actual[0] == expected[0]);
  CHECK(actual[1] == expected[1]);
}

}

TEST_CASE("Territory availability blocks alive bodies and ignores dead bodies") {
  Position position = make_position();

  const Square alive_tail = idx(1, 1);
  const Square alive_middle = idx(1, 2);
  const Square alive_head = idx(1, 3);
  make_snake(position.snakes[0], {alive_tail, alive_middle, alive_head});

  const Square dead_tail = idx(5, 5);
  const Square dead_head = idx(5, 6);
  make_snake(position.snakes[1], {dead_tail, dead_head}, false);

  position.snake_count = 2;
  position.build_occupied();

  engine::evaluation::TerritoryAvailability availability =
      engine::evaluation::initialize_availability(position);

  CHECK_FALSE(availability.available.test(alive_tail));
  CHECK_FALSE(availability.available.test(alive_middle));
  CHECK_FALSE(availability.available.test(alive_head));
  CHECK(availability.available.test(dead_tail));
  CHECK(availability.available.test(dead_head));

  engine::evaluation::release_body_squares(position, availability);
  CHECK(availability.available.test(alive_tail));
  CHECK_FALSE(availability.available.test(alive_middle));
  CHECK_FALSE(availability.available.test(alive_head));

  engine::evaluation::release_body_squares(position, availability);
  CHECK(availability.available.test(alive_middle));
  CHECK_FALSE(availability.available.test(alive_head));

  engine::evaluation::release_body_squares(position, availability);
  CHECK(availability.available.test(alive_head));
}

TEST_CASE("Territory frontiers start at heads but do not mark heads reached") {
  Position position = make_position();

  const Square tail = idx(4, 4);
  const Square head = idx(4, 5);
  make_snake(position.snakes[0], {tail, head});
  position.snake_count = 1;
  position.build_occupied();

  engine::evaluation::TerritoryFrontiers frontiers =
      engine::evaluation::initialize_frontiers(position);

  CHECK(frontiers.current[0].test(head));
  CHECK_FALSE(frontiers.current[0].test(tail));
  CHECK(frontiers.reached[0].empty());
}

TEST_CASE("Territory frontier expansion can enter a tail released on turn one") {
  Position position = make_position();

  const Square tail = idx(4, 4);
  const Square head = idx(4, 5);
  make_snake(position.snakes[0], {tail, head});
  position.snake_count = 1;
  position.build_occupied();

  engine::evaluation::TerritoryAvailability availability =
      engine::evaluation::initialize_availability(position);
  engine::evaluation::TerritoryFrontiers frontiers =
      engine::evaluation::initialize_frontiers(position);

  engine::evaluation::release_body_squares(position, availability);
  engine::evaluation::expand_frontiers(position, availability, frontiers);

  CHECK(frontiers.next[0].test(tail));
  CHECK(frontiers.next[0].test(idx(3, 5)));
  CHECK(frontiers.next[0].test(idx(5, 5)));
  CHECK(frontiers.next[0].test(idx(4, 6)));
  CHECK_FALSE(frontiers.next[0].test(head));
}

TEST_CASE("Territory frontier expansion requires exact movement and does not wait in place") {
  Position position = make_position();

  const Square source = idx(5, 5);
  const Square delayed_target = idx(5, 6);
  make_snake(position.snakes[0], {source});
  position.snake_count = 1;
  position.build_occupied();

  engine::evaluation::TerritoryAvailability availability =
      engine::evaluation::initialize_availability(position);
  engine::evaluation::TerritoryFrontiers frontiers =
      engine::evaluation::initialize_frontiers(position);

  engine::evaluation::release_body_squares(position, availability);
  availability.available.clear(delayed_target);
  engine::evaluation::expand_frontiers(position, availability, frontiers);
  CHECK_FALSE(frontiers.next[0].test(source));
  CHECK_FALSE(frontiers.next[0].test(delayed_target));

  engine::evaluation::advance_frontiers(frontiers);
  availability.available.set(delayed_target);
  engine::evaluation::expand_frontiers(position, availability, frontiers);
  CHECK_FALSE(frontiers.next[0].test(delayed_target));

  engine::evaluation::advance_frontiers(frontiers);
  engine::evaluation::expand_frontiers(position, availability, frontiers);
  CHECK(frontiers.next[0].test(delayed_target));
}

TEST_CASE("Territory claims contest equal-length same-turn arrivals") {
  Position position = make_position();

  const Square target = idx(5, 5);
  make_snake(position.snakes[0], {idx(4, 5)});
  make_snake(position.snakes[1], {idx(6, 5)});
  position.snake_count = 2;
  position.build_occupied();

  engine::evaluation::TerritoryFrontiers frontiers{};
  frontiers.next[0].set(target);
  frontiers.next[1].set(target);

  engine::evaluation::TerritoryClaims claims = engine::evaluation::initialize_claims();
  engine::evaluation::record_arrivals(position, frontiers, claims, 1);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::count_control(position, claims);

  CHECK(control[0] == 0);
  CHECK(control[1] == 0);
}

TEST_CASE("Territory claims award same-turn arrivals to the longer snake") {
  Position position = make_position();

  const Square target = idx(5, 5);
  make_snake(position.snakes[0], {idx(4, 3), idx(4, 4), idx(4, 5)});
  make_snake(position.snakes[1], {idx(6, 5)});
  position.snake_count = 2;
  position.build_occupied();

  engine::evaluation::TerritoryFrontiers frontiers{};
  frontiers.next[0].set(target);
  frontiers.next[1].set(target);

  engine::evaluation::TerritoryClaims claims = engine::evaluation::initialize_claims();
  engine::evaluation::record_arrivals(position, frontiers, claims, 1);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::count_control(position, claims);

  CHECK(control[0] == 1);
  CHECK(control[1] == 0);
}

TEST_CASE("Territory claims ignore later arrivals after an earlier claim") {
  Position position = make_position();

  const Square target = idx(5, 5);
  make_snake(position.snakes[0], {idx(4, 5)});
  make_snake(position.snakes[1], {idx(6, 5), idx(7, 5), idx(8, 5)});
  position.snake_count = 2;
  position.build_occupied();

  engine::evaluation::TerritoryFrontiers frontiers{};
  engine::evaluation::TerritoryClaims claims = engine::evaluation::initialize_claims();

  frontiers.next[0].set(target);
  engine::evaluation::record_arrivals(position, frontiers, claims, 1);

  frontiers.next[0] = BitBoard{};
  frontiers.next[1].set(target);
  engine::evaluation::record_arrivals(position, frontiers, claims, 2);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::count_control(position, claims);

  CHECK(control[0] == 1);
  CHECK(control[1] == 0);
}

TEST_CASE("Territory control gives a single alive length-one snake the whole board") {
  Position position = make_position();

  make_snake(position.snakes[0], {idx(5, 5)});
  position.snake_count = 1;
  position.build_occupied();

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);

  CHECK(control[0] == BOARD_SQUARES);
  CHECK(control[1] == 0);
}

TEST_CASE("Territory control ignores dead snakes and dead bodies") {
  Position position = make_position();

  make_snake(position.snakes[0], {idx(0, 0)});
  make_snake(position.snakes[1], {idx(5, 5), idx(5, 6), idx(5, 7)}, false);
  position.snake_count = 2;
  position.build_occupied();

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);

  CHECK(control[0] == BOARD_SQUARES);
  CHECK(control[1] == 0);
}

TEST_CASE("Territory control splits a symmetric board and contests equal arrivals") {
  Position position = make_position();

  make_snake(position.snakes[0], {idx(0, 5)});
  make_snake(position.snakes[1], {idx(10, 5)});
  position.snake_count = 2;
  position.build_occupied();

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);

  CHECK(control[0] == 55);
  CHECK(control[1] == 55);
}

TEST_CASE("Territory control awards equal-arrival squares to the longer snake") {
  Position position = make_position();

  make_snake(position.snakes[0], {idx(0, 3), idx(0, 4), idx(0, 5)});
  make_snake(position.snakes[1], {idx(10, 5)});
  position.snake_count = 2;
  position.build_occupied();

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);

  CHECK(control[0] == 66);
  CHECK(control[1] == 55);
}

TEST_CASE("Territory control handles intertwined bodies with delayed openings") {
  Position position = make_position();

  make_snake(
      position.snakes[0],
      {idx(2, 2), idx(3, 2), idx(4, 2), idx(4, 3), idx(4, 4), idx(5, 4), idx(6, 4)}
  );
  make_snake(
      position.snakes[1],
      {idx(8, 8), idx(7, 8), idx(6, 8), idx(6, 7), idx(6, 6), idx(5, 6), idx(4, 6)}
  );
  position.snake_count = 2;
  position.build_occupied();

  check_matches_reference(position);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);
  CHECK(control[0] > 0);
  CHECK(control[1] > 0);
  CHECK(control[0] + control[1] <= BOARD_SQUARES);
}

TEST_CASE("Territory control handles a temporary body wall across the center") {
  Position position = make_position();

  make_snake(
      position.snakes[0],
      {idx(5, 0), idx(5, 1), idx(5, 2), idx(5, 3), idx(5, 4), idx(5, 5), idx(4, 5)}
  );
  make_snake(position.snakes[1], {idx(10, 5), idx(9, 5), idx(8, 5)});
  position.snake_count = 2;
  position.build_occupied();

  check_matches_reference(position);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);
  CHECK(control[0] != control[1]);
  CHECK(control[0] + control[1] <= BOARD_SQUARES);
}

TEST_CASE("Territory control handles opposite-corner long bodies") {
  Position position = make_position();

  make_snake(
      position.snakes[0],
      {idx(0, 0), idx(1, 0), idx(2, 0), idx(2, 1), idx(2, 2), idx(3, 2), idx(4, 2), idx(4, 3)}
  );
  make_snake(
      position.snakes[1],
      {idx(10, 10), idx(9, 10), idx(8, 10), idx(8, 9), idx(8, 8), idx(7, 8), idx(6, 8), idx(6, 7)}
  );
  position.snake_count = 2;
  position.build_occupied();

  check_matches_reference(position);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);
  CHECK(control[0] == control[1]);
  CHECK(control[0] + control[1] < BOARD_SQUARES);
}

TEST_CASE("Territory control handles asymmetric long bodies near the same region") {
  Position position = make_position();

  make_snake(
      position.snakes[0],
      {idx(2, 5), idx(2, 4), idx(3, 4), idx(4, 4), idx(5, 4), idx(5, 5), idx(5, 6), idx(6, 6), idx(7, 6)}
  );
  make_snake(
      position.snakes[1],
      {idx(8, 3), idx(8, 4), idx(8, 5), idx(8, 6), idx(8, 7), idx(7, 7)}
  );
  position.snake_count = 2;
  position.build_occupied();

  check_matches_reference(position);

  const engine::evaluation::TerritoryControl control =
      engine::evaluation::compute_territory_control(position);
  CHECK(control[1] > control[0]);
  CHECK(control[0] + control[1] <= BOARD_SQUARES);
}
