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

void chat_client() {

    mqd_t server_connection;
    while (1) {
        server_connection = mq_open("/chat_server_connection", O_RDWR);
        if (server_connection == (mqd_t)-1) {
            if (errno == ENOENT) {
                printf("failed to connect to server\n");
                sleep(3);
                continue;
            }
            else {
                perror("failed to open queue");
                exit(1);
            }
        }
        else {
            printf("sent the connection message to the server\n");
            break;
        }
    }

    char username[100];
    username[0] = '/';
    fgets(username + 1, 100, stdin);
    *strchr(username, '\n') = '\0';
//    mqd_t input_queue = mq_open(username, O_RDWR | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR |
//            S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);
    mqd_t input_queue = mq_open(username, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR |
                        S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);

    struct mq_attr attr;
    mq_getattr(server_connection, &attr);
    char* buf = malloc((unsigned long)attr.mq_msgsize);
    if (buf == NULL) {
        perror("failed to allocate message buffer");
        exit(1);
    }
    mq_send(server_connection, username + 1, strlen(username) + 1, 2);

    while (1) {
        strncpy(buf, username + 1, strlen(username + 1));
        buf[strlen(username + 1)] = ':';
        fgets(buf + strlen(username + 1) + 1,
              (int)attr.mq_msgsize - (int)strlen(username + 1) - 1, stdin);
        *strchr(buf + strlen(username + 1) + 1, '\n') = '\0';
        if (buf[strlen(username + 1) + 1] == '/') {
            mq_send(server_connection, username, 0, 3);
            break;
        }
        else {
            int res = mq_send(server_connection, buf, strlen(buf) + 1, 1);
            if (res < 0) {
                perror("failed to send message");
                exit(1);
            }
            printf("message sent\n");
        }
        unsigned int prio;
//        while (mq_receive(input_queue, buf,
//                       (size_t)attr.mq_msgsize, &prio) == 0) {
//            // getting updates
//            printf("****\n%s\n****\n", buf);
//        }
        long res = mq_receive(input_queue, buf, (size_t)attr.mq_msgsize, &prio);
        if (res < 0) {
            perror("failed message receive");
        }
        printf("****\n%s\n****\n", buf);

    }

    mq_unlink(username);
    mq_close(input_queue);
    free(buf);
}
