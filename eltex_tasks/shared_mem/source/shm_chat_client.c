#include "shm_tasks.h"

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include "chat.h"

typedef struct {
    char* input_buf;
    pthread_mutex_t* p_mutex;
    size_t buf_size;
    int flag;

} input_routine_arg_t;

//void* input_routine(void* _arg) {
//    input_routine_arg_t* arg = (input_routine_arg_t*)_arg;
//    while(1) {
//        pthread_mutex_lock(arg->p_mutex);
//        //the thread will die when the main thread dies so no need to break ever
//        fgets(arg->input_buf, (int)arg->buf_size, stdin);
//        *strchr(arg->input_buf, '\n') = '\0';
////        pthread_mutex_lock(arg->p_mutex);
//        arg->flag = 0;
//        pthread_mutex_unlock(arg->p_mutex);

//        while (arg->flag == 0) {
//            sleep(1);
//        }

//    }
//}

void shm_chat_client() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }

    shm_chat_t* shm_ptr = mmap(NULL, sizeof (shm_chat_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    input_routine_arg_t input_routine_arg;
    input_routine_arg.flag = 1;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    input_routine_arg.p_mutex = &mutex;
    input_routine_arg.buf_size = MESSAGE_MAX;
    input_routine_arg.input_buf = malloc(MESSAGE_MAX);
    if (input_routine_arg.input_buf == NULL) {
        perror("input routine arg malloc");
        exit(1);
    }

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

    size_t client_id = 0;

    sem_wait(&shm_ptr->sem);
    if (shm_ptr->message_new == 0) {
        shm_ptr->message_new = 1;
        strcpy(shm_ptr->msg_buf, "/new");
        client_id = shm_ptr->client_count;
    }
    sem_post(&shm_ptr->sem);
    sem_post(&shm_ptr->sem);



    char* receive_buf = malloc(MESSAGE_MAX);
    while(1) {
        sem_wait(&shm_ptr->sem);
        if (input_routine_arg.flag == 0) {

            if (shm_ptr->message_new == 0) {
                strcpy(shm_ptr->msg_buf, input_routine_arg.input_buf);
                shm_ptr->message_new = 1;
            }

            input_routine_arg.flag = 1;


        }

        if (shm_ptr->client_status[client_id] == 0 ) {
            if (shm_ptr->message_new == 1 ) {

                strcpy(receive_buf, shm_ptr->msg_buf);
                shm_ptr->client_status[client_id] = 1;
                ++shm_ptr->msg_recvd_clients;
                printf(">>> %s\n", receive_buf);
                if (receive_buf[0] == '/' && strcmp(receive_buf + 1, "new") != 0) {
                    break;
                }
            }

        }
        sem_post(&shm_ptr->sem);
//        sleep(1);
    }
    free(input_routine_arg.input_buf);
    free(receive_buf);
}
