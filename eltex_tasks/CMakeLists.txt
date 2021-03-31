cmake_minimum_required(VERSION 2.8)

project(eltex_tasks)

FILE(GLOB MyCSources ./source/*.c ./include/*.h)

add_executable(${PROJECT_NAME} ${MyCSources})

# spiral matrix task and related funcs
add_subdirectory(spiral_matrix)
target_link_libraries(${PROJECT_NAME} spiral_matrix)
include_directories(${PROJECT_NAME} ./spiral_matrix/include)

# phonebook task
add_subdirectory(phonebook)
target_link_libraries(${PROJECT_NAME} phonebook)
include_directories(${PROJECT_NAME} ./phonebook/include)

#bytes_access task
add_subdirectory(modifying_bytes)
target_link_libraries(${PROJECT_NAME} modifying_bytes)
include_directories(${PROJECT_NAME} ./modifying_bytes/include)

#struct task
add_subdirectory(struct_task)
target_link_libraries(${PROJECT_NAME} struct_task)
include_directories(${PROJECT_NAME} ./struct_task/include)

#file tasks
add_subdirectory(file_manager)
target_link_libraries(${PROJECT_NAME} file_manager)
include_directories(${PROJECT_NAME} ./file_manager/include)

#fork and shell tasks
add_subdirectory(fork_task)
target_link_libraries(${PROJECT_NAME} fork_task)
include_directories(${PROJECT_NAME} ./fork_task/include)

#threads tasks
add_subdirectory(threads_task)
target_link_libraries(${PROJECT_NAME} threads_task)
include_directories(${PROJECT_NAME} ./threads_task/include)

#message queue and chat
add_subdirectory(msg_queue)
target_link_libraries(${PROJECT_NAME} msg_queue)
include_directories(${PROJECT_NAME} ./msg_queue/include)

include_directories(${PROJECT_NAME} ./include)


