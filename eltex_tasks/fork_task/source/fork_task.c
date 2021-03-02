#include "shell.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void print_introduction() {
    printf("I am %d, My parent is %d\n", getpid(), getppid());

}

void grandchild() {
    print_introduction();
    exit(0);
}

void child_1() {
    print_introduction();
    pid_t pid = fork();
    if (pid == 0) {
        grandchild();
    }

    pid_t pid_2 = fork();
    if (pid_2 == 0) {
        grandchild();
    }

    waitpid(pid, NULL, 0);
    waitpid(pid_2, NULL, 0);

    exit(0);
}

void child_2() {
    print_introduction();
    pid_t pid = fork();
    if (pid == 0) {
        grandchild();
    }
    waitpid(pid, NULL, 0);

    exit(0);
}


void fork_task() {
    print_introduction();
    pid_t pid = fork();

    if (pid == 0) {
        child_1();
    }
    pid_t pid_2 = fork();
    if (pid_2 == 0) {
        child_2();
    }

    waitpid(pid, NULL, 0);
    waitpid(pid_2, NULL, 0);

    exit(0);

}
