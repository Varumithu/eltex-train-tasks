cmake_minimum_required(VERSION 3.5)

project(spiral_matrix LANGUAGES C)

include_directories(../print_matrix)

add_executable(spiral_matrix main.c ../print_matrix/print_matrix.c)
