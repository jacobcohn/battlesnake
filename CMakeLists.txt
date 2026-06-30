cmake_minimum_required(VERSION 3.20)

project(battlesnake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Crow REQUIRED)

add_executable(battlesnake
  src/main.cpp
  src/evaluation/evaluate.cpp
  src/evaluation/flood_fill.cpp
  src/evaluation/grid.cpp
  src/evaluation/territory.cpp
  src/logging.cpp
  src/minimax.cpp
  src/play/move.cpp
  src/parse.cpp
  src/play/simulate.cpp
)
target_include_directories(battlesnake PRIVATE include)
target_link_libraries(battlesnake PRIVATE Crow::Crow)

enable_testing()

add_executable(simulate_test
  tests/simulate_test.cpp
  tests/test_helpers.cpp
  src/play/move.cpp
  src/play/simulate.cpp
)
target_include_directories(simulate_test PRIVATE include tests)

add_test(NAME simulate_test COMMAND simulate_test)
