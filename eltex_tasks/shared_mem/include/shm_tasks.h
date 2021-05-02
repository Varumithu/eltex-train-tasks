#ifndef _ELTEX_TASKS_SHARED_MEM_SHM_TASKS_H
#define _ELTEX_TASKS_SHARED_MEM_SHM_TASKS_H

#include <semaphore.h>

#define SHM_NAME "/eltex_tasks_shared_mem"

#define MESSAGE_MAX 4192
#define CLIENT_MAX 10

typedef struct {
    sem_t sem;
    char message_new;
    size_t client_count;
    size_t msg_recvd_clients; // clients increment this when finish reading msg_buf
    //when msg_recvd_clients == client_count we can send new message;
    char client_status[CLIENT_MAX];
    char msg_buf[MESSAGE_MAX];
} shm_chat_t;


void shared_mem_test_1();

void shm_chat_client();

void shm_chat_server();

#endif
