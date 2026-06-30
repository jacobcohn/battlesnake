#pragma once

struct EvaluationParams {
  int reachable_space_weight = 3;
  int territory_control_weight = 12;
  int length_advantage_score = 75;
  int low_health_threshold = 25;
  int low_health_penalty_weight = 15;
};
