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

typedef struct {
    sem_t sem;
    char buf[100];
} shm_test_t;

void test_1_child() {
    int shmem_fd = 0;
    while (1) {
        shmem_fd = shm_open(SHM_NAME, O_RDWR, 0666);
        if (shmem_fd < 0) {
            if (errno != ENOENT) {
                perror("shm open in child");
                exit(1);
            }
            sleep(1);
        }
        else
            break;
    }
    sleep(1); // a stupid placeholder to ensure that stuff is ready
    shm_test_t* shm_ptr = mmap(NULL, sizeof (shm_test_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    if (sem_wait(&shm_ptr->sem) < 0) {
        exit(1);
    }
    printf("%s", shm_ptr->buf);
    exit(0);
}

void shared_mem_test_1() {
    shm_unlink(SHM_NAME);
    int child_pid = fork();
    if (child_pid == 0) {
        test_1_child();
    }

    int shmem_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (shmem_fd < 0) {
        perror("shm_open");
        kill(child_pid, SIGKILL);
        exit(1);
    }
    char* test_string = "blah blah blah text blah";
    if (ftruncate(shmem_fd, (off_t)strlen(test_string) + 1) < 0) {
        perror("ftruncate");
        kill(child_pid, SIGKILL);
        exit(1);
    }
    shm_test_t* shm_ptr = mmap(NULL, sizeof (shm_test_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
    if (sem_init(&shm_ptr->sem, 1, 0) < 0) {
        perror("sem_init");
        kill(child_pid, SIGKILL);
        exit(1);
    }
    strcpy(shm_ptr->buf, test_string);
    sem_post(&shm_ptr->sem);

}
