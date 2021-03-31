#include "chat.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

void sender(){
    mqd_t mq = mq_open("/task1_queue",
                       O_RDWR | O_CREAT,
                       S_IRUSR | S_IWUSR |
                       S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);

    if (mq == (mqd_t)-1) {
        perror("failed to create queue");
        exit(1);
    }
    char* message = "message sent through posix queue";
    mq_send(mq, message, strlen(message) + 1, 0);
    mq_close(mq);

    key_t key = ftok("/home/valmit/Documents/QtProjects/eltex_tasks/eltex_tasks/msg_queue/source/msg_task1.c", 1212);
    if (key == (key_t)-1) {
        perror("creating key");
        exit(1);
    }
    int sysv_mq = msgget(key, IPC_CREAT | 0666);
    if (sysv_mq == -1) {
        perror("failed to create sys V mq");
        exit(1);
    }
    struct msgbuf msg;
    msg.mtype = 1;
    strcpy(msg.mtext, "message sent through System V queue");
    ssize_t len = msgsnd(sysv_mq, &msg, 100, 0);
    if (len == -1) {
        perror("msgsnd");
        exit(1);
    }
}

void receiver() {
    mqd_t mq;
    while (1) {
        mq = mq_open("/task1_queue", O_RDWR);
        if (mq == (mqd_t)-1) {
            if (errno == ENOENT) {
                sleep(3);
                continue;
            }
            else {
                perror("failed to open queue");
                exit(1);
            }
        }
        else {
            break;
        }
    }
    struct mq_attr attr;
    mq_getattr(mq, &attr);
    char* buf = malloc((unsigned long)attr.mq_msgsize);
    mq_receive(mq, buf, (unsigned long)attr.mq_msgsize, NULL);
    printf("%s\n", buf);
    mq_unlink("/task1_queue");
    mq_close(mq);

    key_t key = ftok("/home/valmit/Documents/QtProjects/eltex_tasks/eltex_tasks/msg_queue/source/msg_task1.c", 1212);
    if (key == (key_t)-1) {
        perror("creating key");
        exit(1);
    }
    int sysv_mq = msgget(key, IPC_CREAT | 0666);
    if (sysv_mq == -1) {
        perror("failed to create sys V mq");
        exit(1);
    }
    struct msgbuf msg;
    ssize_t len = msgrcv(sysv_mq, &msg, 100, 0, 0);
    if (len == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("%s", msg.mtext);

    exit(0);
}

void msg_task_1() {
    pid_t pid = fork();
    if (pid == 0) {
        receiver();
    }
    else if (pid < 0) {
        perror("fork");
        exit(1);
    }
    else {
        sender();
    }
}
