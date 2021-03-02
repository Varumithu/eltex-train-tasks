cmake_minimum_required(VERSION 2.8)

project(spiral_matrix)

FILE(GLOB MyCSources ./source/*.c ./include/*.h)

add_library(${PROJECT_NAME} ${MyCSources})

include_directories(${PROJECT_NAME} ./include)

target_link_libraries(${PROJECT_NAME} m)

