#include "threads.h"

#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

void* thread_start(void* p_a) {
    ssize_t a = *(ssize_t*)p_a;


    for (size_t i = 0; i < 100000; ++i) {
        a++;
    }
    void* retval = (void*)(a);
    *(ssize_t*)(p_a) = a;
    pthread_exit(retval);
}

void threads_task_1() {
    ssize_t a = 0;
    size_t thread_amount = 10000;
    pthread_t* thread_ids = (pthread_t*)malloc(thread_amount * sizeof(pthread_t));
    for (size_t i = 0; i < thread_amount; ++i) {
        int s = pthread_create(&(thread_ids[i]), NULL, thread_start, &a);
        if (s != 0) {
            perror("pthread_creat error");
            exit(1);
        }
    }

    for (size_t i = 0; i < thread_amount; ++i) {
        void* cur_a = NULL;
        int s = pthread_join(thread_ids[i], &cur_a);
        if (s != 0) {
            perror("join error");
            exit(1);
        }
        else {
            printf("thread returned %ld, current value of a = %ld\n",
                     (ssize_t)cur_a, a);
        }
    }



}
