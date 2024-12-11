cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(AOC_DAY 11)

project(AOC_2024_${AOC_DAY})

set(APP_NAME day_${AOC_DAY})

add_executable(${APP_NAME} main.cpp)

add_custom_command(
  TARGET ${APP_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test_input.txt
          ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
