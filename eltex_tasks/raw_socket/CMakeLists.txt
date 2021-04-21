cmake_minimum_required(VERSION 2.8)

project(raw_socket)

FILE(GLOB MyCSources ./source/*.c ./include/*.h)

add_library(${PROJECT_NAME} ${MyCSources})

target_link_libraries(${PROJECT_NAME} pthread)

include_directories(${PROJECT_NAME} ./include)
