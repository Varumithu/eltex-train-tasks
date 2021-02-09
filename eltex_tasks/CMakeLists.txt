cmake_minimum_required(VERSION 2.8)

project(eltex_tasks)

FILE(GLOB MyCSources ./source/*.c ./include/*.h)

add_executable(${PROJECT_NAME} ${MyCSources})

include_directories(${PROJECT_NAME} ./include)

target_link_libraries(${PROJECT_NAME} m)


target_link_libraries(${PROJECT_NAME} ncurses)

