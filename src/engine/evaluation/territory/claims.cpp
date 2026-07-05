#include "engine/evaluation/territory/claims.hpp"

namespace engine::evaluation {

TerritoryClaims::TerritoryClaims() noexcept {
  snake.fill(-1);
  turn.fill(BOARD_SQUARES * 2 + 1);
  length.fill(0);
  contested.fill(false);
}

}
