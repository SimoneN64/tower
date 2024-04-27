cmake_minimum_required(VERSION 3.29)
project(tower C)

set(CMAKE_C_STANDARD 11)

add_executable(tower
  main.c
  driver.h
  game.h
  game.c
  util.h
  util.c)
