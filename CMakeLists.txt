cmake_minimum_required(VERSION 3.20)
project(cs6420_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

FILE(GLOB ALG_CPP src/alg_*.cpp)
add_executable(prog1 src/part1.cpp ${ALG_CPP})
add_executable(prog2 src/part2.cpp ${ALG_CPP})

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(tester1 test/tester1.cpp ${ALG_CPP})
target_link_libraries(tester1 PRIVATE Catch2::Catch2WithMain)

add_executable(tester2 test/tester2.cpp ${ALG_CPP})
target_link_libraries(tester2 PRIVATE Catch2::Catch2WithMain)
