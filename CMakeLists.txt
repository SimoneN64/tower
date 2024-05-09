cmake_minimum_required(VERSION 3.20)
project(tower C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer_ext REQUIRED)

add_executable(tower
  main.c
  game.h
  game.c
  util.h
  util.c)

target_include_directories(tower PUBLIC SDL2_INCLUDE_DIRS)
target_link_libraries(tower PUBLIC SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_mixer_ext::SDL2_mixer_ext)