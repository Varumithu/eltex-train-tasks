cmake_minimum_required(VERSION 2.8)

project(socket_task)

FILE(GLOB MyCSources ./source/*.c ./include/*.h)

add_library(${PROJECT_NAME} ${MyCSources})

include_directories(${PROJECT_NAME} ./include)
