#pragma once

#include <array>
#include <string_view>

#include "game_state.h"

enum class Move {
  Up,
  Right,
  Down,
  Left,
};

constexpr std::array<Move, 4> ALL_MOVES = {
  Move::Up,
  Move::Right,
  Move::Down,
  Move::Left,
};

Coordinate apply_move(Coordinate coordinate, Move move);
std::string_view move_to_string(Move move);
