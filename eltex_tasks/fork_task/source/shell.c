#include "shell.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void print_invitation() {
    printf(">>");
}

void execute_command(char*** p_args) {

    if (execvp(**p_args, *p_args) < 0) {
        perror("exec error");
    }
    exit(1);
}

int separate_args(char* line, char*** p_args) {
    size_t alloc_size = 100;
    *p_args = (char**)malloc(sizeof(char**) * alloc_size);
    if (*p_args == NULL) {
        perror("alloc error in separate_args");
        exit(1);
    }
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
        if (arr_pos >= cur_arr_size) {
            cur_arr_size += alloc_size;
            *p_args = realloc(*p_args, cur_arr_size * sizeof(char**));
            if (*p_args == NULL) {
                perror("realloc error in separate_args");
                exit(1);
            }
        }
        (*p_args)[arr_pos] = malloc(length + 1);
        if ((*p_args)[arr_pos] == NULL) {
            perror("string allocation error in separate_args");
            exit(1);
        }
        strncpy((*p_args)[arr_pos],
                line + line_pos,
                length);
        (*p_args)[arr_pos][length] = '\0';
        line_pos += length + 1;
        ++arr_pos;
        if (next_space == NULL) {
            break; // beacause adding length + 1 does not work in this case
        }

    }
    if (arr_pos >= cur_arr_size) {
        cur_arr_size += 1;
        *p_args = realloc(*p_args, cur_arr_size * sizeof(char**));
        if (*p_args == NULL) {
            perror("realloc error in the end of separate_args");
            exit(1);
        }
    }
    (*p_args)[arr_pos] = NULL;
    return 0;
}

void shell_start() {
    size_t line_max = 1000 + 1;
    char* line = (char*)malloc(line_max);
    if (line == NULL) {
        perror("allocation failure");
        exit(1);
    }
    char* ptr = NULL;
    while(1) {
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
        char** args = NULL;
        separate_args(line, &args);
        char** iterator = args;
        while (*iterator != NULL) {
            printf("%s\n", *iterator);
            ++iterator;
        }
        //end debug part
        pid_t pid = fork();
        if (pid == 0) {
            execute_command(&args);
        }

        iterator = args;
        while(*iterator != NULL) {
            free(*iterator);
            ++iterator;
        }
        free(*iterator);
        free(args);
        int wstatus = 0;

        waitpid(pid, &wstatus, 0);
        printf("exitstatus: %d\n", wstatus);
    }

    free(line);
}
