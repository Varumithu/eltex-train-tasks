cmake_minimum_required(VERSION 3.5)

project(untitled1 LANGUAGES C)

include_directories(../print_matrix)

add_executable(untitled1 main.c ../print_matrix/print_matrix.c)
