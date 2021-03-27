#ifndef _FORK_TASK_H
#define _FORK_TASK_H

#include <sys/types.h>

typedef enum  {
    ST_IN,
    PREV_PIPE
} input_flag_t;

typedef enum  {
    ST_OUT,
    TO_PIPE
} output_flag_t;

typedef struct {
    output_flag_t out_flag;
    input_flag_t in_flag;
    char** args; // null_terminated array of strings
    pid_t pid;
    int input_pipe_handle, output_pipe_handle;
} shell_command_t;

void fork_task();

void shell_start();

#endif
