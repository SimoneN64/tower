cmake_minimum_required(VERSION 3.29)
project(tower C)

set(CMAKE_C_STANDARD 11)

add_executable(tower
  tower_main.c
  driver.h
  game.h)
