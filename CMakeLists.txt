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
  src/server/parser.cpp
)
target_include_directories(battlesnake PRIVATE include)
target_link_libraries(battlesnake PRIVATE Crow::Crow)
