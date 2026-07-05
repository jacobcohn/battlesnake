#include "engine/evaluation/territory/territory.hpp"

#include "engine/evaluation/territory/availability.hpp"
#include "engine/evaluation/territory/claims.hpp"
#include "engine/evaluation/territory/frontiers.hpp"

namespace engine::evaluation {

TerritoryControl compute_territory_control(const Position& position) noexcept {
  TerritoryAvailability availability = initialize_availability(position);
  TerritoryFrontiers frontiers = initialize_frontiers(position);
  TerritoryClaims claims{};

  // Heads are movement sources at turn 0, but they are not territory yet.
  // reached remains empty until a snake legally arrives on a later turn.
  release_body_squares(position, availability);

  (void)availability;
  (void)frontiers;
  (void)claims;
  return {};
}

}
