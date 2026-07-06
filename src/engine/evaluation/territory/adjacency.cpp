#include "engine/evaluation/territory/adjacency.hpp"

namespace engine::evaluation {

BitBoard adjacent_squares(BitBoard squares) noexcept {
  return squares.north() | squares.south() | squares.east() | squares.west();
}

}
