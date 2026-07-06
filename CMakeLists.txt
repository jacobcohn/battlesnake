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
  src/engine/evaluation/evaluation.cpp
  src/engine/evaluation/territory/territory.cpp
  src/engine/evaluation/territory/adjacency.cpp
  src/engine/evaluation/territory/availability.cpp
  src/engine/evaluation/territory/claims.cpp
  src/engine/evaluation/territory/frontiers.cpp
  src/engine/move/apply.cpp
  src/engine/move/collisions.cpp
  src/engine/move/execution.cpp
  src/engine/move/generation.cpp
  src/engine/move/undo.cpp
  src/engine/search/context.cpp
  src/engine/search/fallback.cpp
  src/engine/search/minimax.cpp
  src/engine/search/root.cpp
  src/engine/search/search.cpp
  src/engine/search/terminal.cpp
  src/server/parser.cpp
  src/server/logging.cpp
)
target_include_directories(battlesnake PRIVATE include)
target_link_libraries(battlesnake PRIVATE Crow::Crow)

include(CTest)

if(BUILD_TESTING)
  find_package(doctest CONFIG REQUIRED)

  add_executable(battlesnake_tests
    tests/test_main.cpp
    tests/engine/core/test_bitboard.cpp
    tests/engine/core/test_move.cpp
    tests/engine/core/test_position.cpp
    tests/engine/core/test_snake.cpp
    tests/engine/core/test_square.cpp
    tests/engine/evaluation/test_evaluation.cpp
    tests/engine/evaluation/territory/test_territory.cpp
    tests/engine/move/test_execution.cpp
    tests/engine/move/test_generation.cpp
    tests/engine/search/test_search.cpp
    tests/server/test_parser.cpp
    src/engine/evaluation/evaluation.cpp
    src/engine/evaluation/territory/territory.cpp
    src/engine/evaluation/territory/adjacency.cpp
    src/engine/evaluation/territory/availability.cpp
    src/engine/evaluation/territory/claims.cpp
    src/engine/evaluation/territory/frontiers.cpp
    src/engine/move/apply.cpp
    src/engine/move/collisions.cpp
    src/engine/move/execution.cpp
    src/engine/move/generation.cpp
    src/engine/move/undo.cpp
    src/engine/search/context.cpp
    src/engine/search/fallback.cpp
    src/engine/search/minimax.cpp
    src/engine/search/root.cpp
    src/engine/search/search.cpp
    src/engine/search/terminal.cpp
    src/server/parser.cpp
    src/server/logging.cpp
  )
  target_include_directories(battlesnake_tests PRIVATE include)
  target_link_libraries(battlesnake_tests PRIVATE doctest::doctest Crow::Crow)

  add_test(NAME battlesnake_tests COMMAND battlesnake_tests)
endif()
