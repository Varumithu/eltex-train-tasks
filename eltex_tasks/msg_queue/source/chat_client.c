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
#include <pthread.h>

typedef struct {
    char* input_buf;
    pthread_mutex_t* p_mutex;
    size_t buf_size;
    int flag;

} input_routine_arg_t;

void* input_routine(void* _arg) {
    input_routine_arg_t* arg = (input_routine_arg_t*)_arg;
    while(1) {
        pthread_mutex_lock(arg->p_mutex);
        //the thread will die when the main thread dies so no need to break ever
        fgets(arg->input_buf, (int)arg->buf_size, stdin);
        *strchr(arg->input_buf, '\n') = '\0';
//        pthread_mutex_lock(arg->p_mutex);
        arg->flag = 0;
        pthread_mutex_unlock(arg->p_mutex);

        while (arg->flag == 0) {
            sleep(1);
        }

    }
}

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
            printf("opened server connection queue\n");
            break;
        }
    }


    char username[100];
    username[0] = '/';
    fgets(username + 1, 100, stdin);
    *strchr(username, '\n') = '\0';
//    mqd_t input_queue = mq_open(username, O_RDWR | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR |
//            S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);
    mqd_t input_queue = mq_open(username, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR |
                        S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);

    if (input_queue == (mqd_t)(-1)) {
        perror("failed to open input queue");
        exit(1);
    }
    struct mq_attr attr;
    mq_getattr(server_connection, &attr);
    char* buf = malloc((unsigned long)attr.mq_msgsize);
    if (buf == NULL) {
        perror("failed to allocate message buffer");
        exit(1);
    }

    strncpy(buf, username + 1, strlen(username + 1));
    buf[strlen(username + 1)] = ':';
    mq_send(server_connection, username + 1, strlen(username) + 1, 2);

    char* mq_receive_buf = malloc((unsigned long)attr.mq_msgsize);
    if (mq_receive_buf == NULL) {
        perror("failed to allocate message buffer");
        exit(1);
    }

    input_routine_arg_t input_routine_arg;
    input_routine_arg.flag = 1;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    input_routine_arg.p_mutex = &mutex;
    input_routine_arg.buf_size = (size_t)attr.mq_msgsize - (size_t)strlen(username + 1) - 1;
    input_routine_arg.input_buf = buf + strlen(username + 1) + 1;

    pthread_t thread_id;
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    int s = pthread_create(&thread_id, &thread_attr, input_routine, (void*)(&input_routine_arg));
    if (s != 0) {
        perror("pthread_create error");
        exit(1);
    }
    pthread_attr_destroy(&thread_attr);
    while (1) {
        if (input_routine_arg.flag == 0 && pthread_mutex_trylock(&mutex) == 0) {
            if (buf[strlen(username + 1) + 1] == '/') {
                mq_send(server_connection, username, 0, 3);
            }
            else {
                int res = mq_send(server_connection, buf, strlen(buf) + 1, 1);
                if (res < 0) {
                    perror("failed to send message");
                    exit(1);
                }
                printf("message sent\n");
            }
            input_routine_arg.flag = 1;
            pthread_mutex_unlock(&mutex);
        }


        unsigned int prio = 0;

        mq_getattr(input_queue, &attr);
        while (attr.mq_curmsgs > 0) {
            long res = mq_receive(input_queue, mq_receive_buf, (size_t)attr.mq_msgsize, &prio);
            if (prio == 3) {
                break;
            }
            if (res < 0) {
                perror("failed message receive");
            }
            printf(">>>>%s\n", mq_receive_buf);
            mq_getattr(input_queue, &attr);
        }
        if (prio == 3) {
            break;
        }

    }

    mq_unlink(username);
    mq_close(input_queue);
    free(buf);
    free(mq_receive_buf);
}
