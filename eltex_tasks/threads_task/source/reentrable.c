#include "threads.h"

#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static pthread_key_t key;
pthread_once_t once_control = PTHREAD_ONCE_INIT;

void key_destructor(void* ptr) {
    pthread_key_t* p_key = ptr;
    char* str = pthread_getspecific(*p_key);
    free(str);
}

void make_key() {
    pthread_key_create(&key, key_destructor);
}

char* err_str(int errnum) {
    pthread_once(&once_control, make_key);
    char* ptr = pthread_getspecific(key);
    if (ptr == NULL) {
        ptr = malloc(100);
        pthread_setspecific(key, ptr);
    }
    switch (errnum) {
    case 0:
        strcpy(ptr, "out of memory");
        break;
    case 1:
        strcpy(ptr, "could not open file");
        break;
    default:
        strcpy(ptr, "not a valid error number");
    }
    return ptr;
}

void err_str_test1() {
    for (size_t i = 0; i < 3; ++i) {
        printf("%s\n", err_str((int)i));
    }
}

