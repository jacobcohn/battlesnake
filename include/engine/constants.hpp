#pragma once

inline constexpr int BOARD_WIDTH = 11;
inline constexpr int BOARD_HEIGHT = 11;
inline constexpr int BOARD_SQUARES = BOARD_WIDTH * BOARD_HEIGHT;
inline constexpr int MAX_SNAKES = 2;

static_assert(BOARD_SQUARES == 121, "Battlesnake engine currently assumes an 11x11 board");
