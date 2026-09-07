#include <exception>

#include <crow.h>

#include "engine/search/search.hpp"
#include "server/parser.hpp"
// #include "server/logging.hpp"
#include "server/response.hpp"

namespace {

constexpr int SEARCH_MAX_DEPTH = 100;
constexpr int SEARCH_TIME_LIMIT_MS = 300;

}

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

  try {
    Position position = parse_position(body);

    engine::search::SearchLimits limits;
    limits.max_depth = SEARCH_MAX_DEPTH;
    limits.time_limit_ms = SEARCH_TIME_LIMIT_MS;

    const engine::search::SearchResult result = engine::search::search(position, limits);
    // log_search_result(position, result);

    crow::json::wvalue response_body = make_move_response(position, result);
    crow::response response{response_body.dump()};
    response.set_header("Content-Type", "application/json");
    return response;

  } catch (const std::exception& exception) {
    crow::json::wvalue response_body;
    response_body["error"] = exception.what();

    crow::response response{400, response_body.dump()};
    response.set_header("Content-Type", "application/json");
    return response;
  }
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
