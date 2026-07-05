#include "engine/evaluation/territory/territory.hpp"

#include "engine/evaluation/territory/availability.hpp"
#include "engine/evaluation/territory/claims.hpp"
#include "engine/evaluation/territory/frontiers.hpp"

namespace engine::evaluation {

namespace {

constexpr int MAX_TERRITORY_TURNS = BOARD_SQUARES * 2;

}

TerritoryControl compute_territory_control(const Position& position) noexcept {
  TerritoryAvailability availability = initialize_availability(position);
  TerritoryFrontiers frontiers = initialize_frontiers(position);
  TerritoryClaims claims = initialize_claims();

  for (int turn = 1; turn <= MAX_TERRITORY_TURNS; ++turn) {
    release_body_squares(position, availability);
    expand_frontiers(position, availability, frontiers);

    if (next_frontiers_empty(position, frontiers)) {
      break;
    }

    record_arrivals(position, frontiers, claims, turn);

    if (all_squares_resolved(claims)) {
      break;
    }

    advance_frontiers(frontiers);
  }

  return count_control(position, claims);
}

}
