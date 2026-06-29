#include <crow.h>

#include <iostream>

#include "minimax.h"
#include "move.h"
#include "parse.h"

constexpr int SEARCH_DEPTH = 4;

crow::response get_info() {
  crow::json::wvalue info;
  info["apiversion"] = "1";
  info["color"] = "#3F51B5";
  info["head"] = "silly";
  info["tail"] = "hook";

  crow::response response{info.dump()};
  response.set_header("Content-Type", "application/json");
  return response;
}

void log_minimax_stats(Move selected_move, const MinimaxStats& minimax_stats) {
  std::cout << "minimax depth=" << minimax_stats.depth
    << " nodes=" << minimax_stats.nodes_evaluated
    << " elapsed_ms=" << minimax_stats.elapsed_ms
    << " selected=" << move_to_string(selected_move)
    << " scores=";

  for (const MinimaxMoveScore& move_score : minimax_stats.top_level_scores) {
    std::cout << move_to_string(move_score.move) << "=" << move_score.score << " ";
  }

  std::cout << std::endl;
}

crow::response start(const crow::request& request) {
  crow::json::rvalue body = crow::json::load(request.body);

  if (!body) {
    return crow::response{400, "invalid JSON"};
  }

  return crow::response{};
}

crow::response move(const crow::request& request) {
  crow::json::rvalue body = crow::json::load(request.body);

  if (!body) {
    return crow::response{400, "invalid JSON"};
  }

  GameState state = parse_game_state(body);
  MinimaxStats minimax_stats;
  Move selected_move = choose_minimax_move(state, SEARCH_DEPTH, minimax_stats);

  log_minimax_stats(selected_move, minimax_stats);

  crow::json::wvalue response_body;
  response_body["move"] = std::string(move_to_string(selected_move));

  crow::response response{response_body.dump()};
  response.set_header("Content-Type", "application/json");
  return response;
}

crow::response end(const crow::request& request) {
  crow::json::rvalue body = crow::json::load(request.body);

  if (!body) {
    return crow::response{400, "invalid JSON"};
  }

  return crow::response{};
}

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")(get_info);
  CROW_ROUTE(app, "/start").methods(crow::HTTPMethod::POST)(start);
  CROW_ROUTE(app, "/move").methods(crow::HTTPMethod::POST)(move);
  CROW_ROUTE(app, "/end").methods(crow::HTTPMethod::POST)(end);

  app.port(8080).run();
}
