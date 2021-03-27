#include "shell.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

void print_invitation() {
    printf(">>");
}

void execute_command(shell_command_t* p_command) {

    if (p_command->in_flag == PREV_PIPE) {
        dup2(p_command->input_pipe_handle, STDIN_FILENO);
    }
    if (p_command->out_flag == TO_PIPE) {
        dup2(p_command->output_pipe_handle, STDOUT_FILENO);
    }
    if (execvp(*(p_command->args), p_command->args) < 0) {
        perror("exec error");
    }
    exit(1);
}

int separate_args(char* line, shell_command_t* p_command) {
    size_t alloc_size = 100;
    p_command->args = (char**)malloc(sizeof(char**) * alloc_size);
    if (p_command->args == NULL) {
        perror("alloc error in separate_args");
        exit(1);
    }
    int value_to_return = 0;
    size_t line_pos = 0;
    size_t arr_pos = 0;
    size_t cur_arr_size = alloc_size;
    while(line[line_pos != '\0']) {
        char* next_space = strchr(line + line_pos, ' '); // maybe later make it search for any isspace character
        size_t length = 0;
        if (next_space == NULL) {
            length = strlen(line + line_pos);
        }
        else {
            length = (size_t)labs(next_space - (line + line_pos));
        }

        // if we encounter '|' we break, set ret_val to 1 and do not add it to args array
        if (strncmp("|", line + line_pos, length) == 0) {
            value_to_return = 1;
            break;
        }

        if (arr_pos >= cur_arr_size) {
            cur_arr_size += alloc_size;
            p_command->args = realloc(p_command->args, cur_arr_size * sizeof(char**));
            if (p_command->args == NULL) {
                perror("realloc error in separate_args");
                exit(1);
            }
        }
        (p_command->args)[arr_pos] = malloc(length + 1);
        if ((p_command->args)[arr_pos] == NULL) {
            perror("string allocation error in separate_args");
            exit(1);
        }
        strncpy((p_command->args)[arr_pos],
                line + line_pos,
                length);
        (p_command->args)[arr_pos][length] = '\0';
        line_pos += length + 1;
        ++arr_pos;
        if (next_space == NULL) {
            break; // beacause adding length + 1 does not work in this case
        }

    }
    if (arr_pos >= cur_arr_size) {
        cur_arr_size += 1;
        p_command->args = realloc(p_command->args, cur_arr_size * sizeof(char**));
        if (p_command->args == NULL) {
            perror("realloc error in the end of separate_args");
            exit(1);
        }
    }
    (p_command->args)[arr_pos] = NULL;
    return value_to_return;
}

char* trim_line(char* line, shell_command_t* command) {

    int length = 0;
    for (char** iterator = command->args; *iterator != NULL; ++iterator) {
        length += strlen(*iterator);
    }
    line += length + 1;
    while ((!isalnum(*line)) && (*line != '\0')) {
        ++line;
    }
    return line;
}


void set_command_io_flags(shell_command_t* command_arr,
                          int separate_arg_val, size_t command_idx)
{
    shell_command_t* p_cur_command = command_arr + command_idx;
    if (command_idx == 0) {
        p_cur_command->in_flag = ST_IN;
    }
    else {
        p_cur_command->in_flag = PREV_PIPE;
    }

    if (separate_arg_val == 0) {
        p_cur_command->out_flag = ST_OUT;

    }
    else {
        p_cur_command->out_flag = TO_PIPE;
    }

}

void set_pipes(shell_command_t* command, int* pipefd, int* prev_pipefd) {
    int* pipe_in = (int*)pipefd + 1;
    int*  prev_pipe_out = (int*)prev_pipefd;

    if (command->in_flag == PREV_PIPE) {
        command->input_pipe_handle = *prev_pipe_out;
    }
    if (command->out_flag == TO_PIPE) {
        pipe(pipefd);
        command->output_pipe_handle = *pipe_in;
        memmove(prev_pipefd, pipefd, 2 * sizeof(int));
    }
}

void shell_start() {
    size_t line_max = 1000 + 1;
    char* line = (char*)malloc(line_max);
    char* line_iter = line;
    if (line == NULL) {
        perror("allocation failure");
        exit(1);
    }
    char* ptr = NULL;
    size_t command_array_init_size = 10;
    shell_command_t* command_arr = malloc(command_array_init_size * sizeof(shell_command_t));

    int pipefd[2] = {-1, -1};
    int prev_pipefd[2] = {-1, -1};
//    int* pipe_in = (int*)pipefd + 1, * pipe_out = (int*)pipefd;
//    int* prev_pipe_in = (int*)prev_pipefd + 1, * prev_pipe_out = (int*)prev_pipefd;

    while(1) {
        size_t command_count = 0;
        line_iter = line;
        print_invitation();
        ptr = fgets(line, (int)line_max, stdin);
        if (ptr == NULL) {
            perror("read fail");
            exit(1);
        }
        line[strlen(line) - 1] = '\0'; //remone \n and put \0 there
        if (strcmp(line, "quit_shell") == 0) {
            break;
        }
        //debug separate_args
        printf("\n");
        int ret_val = 1;
        while (ret_val != 0) {
            ret_val = separate_args(line_iter, &(command_arr[command_count]));
            set_command_io_flags(command_arr, ret_val, command_count);
            set_pipes(command_arr + command_count, pipefd, prev_pipefd);


            line_iter = trim_line(line_iter, command_arr + command_count);
            char** iterator = command_arr[command_count].args;
            while (*iterator != NULL) {
                printf("%s\n", *iterator);
                ++iterator;
            }
            printf("\n");
            //end debug part
            command_arr[command_count].pid = fork();
            if (command_arr[command_count].pid == 0) {
                execute_command(&(command_arr[command_count]));
            }
            ++command_count;
        }

        for (size_t i = 0; i < command_count; ++i) {
            char** iterator = command_arr[i].args;
            while(*iterator != NULL) {
                free(*iterator);
                ++iterator;
            }
            free(*iterator);
            free(command_arr[i].args);
            int wstatus = 0;

            waitpid(command_arr[i].pid, &wstatus, 0);
            if (command_arr[i].out_flag == TO_PIPE) {
                write(command_arr[i].output_pipe_handle, "\x04\n\0\x17\x19\x1b", 6);
                close(command_arr[i].output_pipe_handle);
            }
            if (command_arr[i].in_flag == PREV_PIPE) {
                close(command_arr[i].input_pipe_handle);
            }
            printf("command %lu exitstatus: %d\n", i, wstatus);
        }

    }
    free(command_arr);
    free(line);
}
