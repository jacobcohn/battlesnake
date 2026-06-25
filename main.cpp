#include <crow.h>

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

crow::response start_game(const crow::request& request) {
  return crow::response{};
}

crow::response select_move(const crow::request& request) {
  return crow::response{};
}

crow::response end_game(const crow::request& request) {
  return crow::response{};
}

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")(get_info);
  CROW_ROUTE(app, "/start").methods(crow::HTTPMethod::POST)(start_game);
  CROW_ROUTE(app, "/move").methods(crow::HTTPMethod::POST)(select_move);
  CROW_ROUTE(app, "/end").methods(crow::HTTPMethod::POST)(end_game);

  app.port(8080).run();
}
