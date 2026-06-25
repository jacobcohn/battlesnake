#include <crow.h>
#include <string>

std::string hello_world() {
  return "hello world\n";
}

int main() {
  crow::SimpleApp app;
  CROW_ROUTE(app, "/")(hello_world);
  app.port(8080).run();
}
