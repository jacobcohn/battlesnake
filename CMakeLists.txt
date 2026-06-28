cmake_minimum_required(VERSION 3.20)

project(battlesnake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow REQUIRED)

add_executable(battlesnake src/main.cpp src/select_move.cpp src/parse.cpp src/move.cpp src/simulate.cpp)
target_include_directories(battlesnake PRIVATE include)
target_link_libraries(battlesnake PRIVATE Crow::Crow)

enable_testing()

add_executable(simulate_test tests/simulate_test.cpp src/simulate.cpp src/move.cpp)
target_include_directories(simulate_test PRIVATE include)

add_test(NAME simulate_test COMMAND simulate_test)
