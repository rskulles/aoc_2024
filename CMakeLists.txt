cmake_minimum_required(VERSION 3.20)

project(AOC_2024)

add_subdirectory(day_1)
add_subdirectory(day_2)
add_subdirectory(day_3)
add_subdirectory(day_4)
add_subdirectory(day_5)
add_subdirectory(day_6)
add_subdirectory(day_7)
add_subdirectory(day_8)
add_subdirectory(skeleton)
add_custom_target(ADVENT_ALL DEPENDS day_1 day_2 day_3 day_4)
