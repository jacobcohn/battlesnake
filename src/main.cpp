#include <crow.h>

#include "select_move.h"

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

  int board_width = body["board"]["width"].i();
  int board_height = body["board"]["height"].i();
  int head_x = body["you"]["head"]["x"].i();
  int head_y = body["you"]["head"]["y"].i();

  std::string selected_move = select_move(board_width, board_height, head_x, head_y);

  crow::json::wvalue response_body;
  response_body["move"] = selected_move;

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
