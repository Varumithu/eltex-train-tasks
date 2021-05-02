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

void shm_chat_server() {
    shm_unlink(SHM_NAME);
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(shm_fd, sizeof(shm_chat_t)) < 0) {
        perror("ftruncate");
        exit(1);
    }
    shm_chat_t* shm_ptr = mmap(NULL, sizeof (shm_chat_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm_ptr->message_new = 0;
    shm_ptr->client_count = 1;// server is a client
    shm_ptr->msg_recvd_clients = 0;
    memset(shm_ptr->client_status, 0, CLIENT_MAX);
    if (sem_init(&shm_ptr->sem, 1, 1) < 0) {
        perror("sem_init");
        exit(1);
    }

    while (1) {
        sem_wait(&shm_ptr->sem);



        if (shm_ptr->client_status[0] == 0 &&
                shm_ptr->message_new == 1) {
            if (shm_ptr->msg_buf[0] == '/') {

                if (strcmp(shm_ptr->msg_buf + 1, "new") == 0) {
                    // new client
                    ++shm_ptr->client_count;
                    if (shm_ptr->client_count > CLIENT_MAX) {
                        exit(1);
                    }
                }
                else {
                    //everything else means close server
                    //clients will read the same message and now they should stop
                    break;
                }
            }
            shm_ptr->client_status[0] = 1;
            ++shm_ptr->msg_recvd_clients;
        }
        if (shm_ptr->msg_recvd_clients == shm_ptr->client_count) {
            shm_ptr->message_new = 0;
            memset(shm_ptr->client_status, 0, shm_ptr->client_count);
            shm_ptr->msg_recvd_clients = 0;
        }

        sem_post(&shm_ptr->sem);


//        sleep(1);
    }

    munmap(shm_ptr, sizeof(shm_chat_t));
    shm_unlink(SHM_NAME);
    close(shm_fd);
}
