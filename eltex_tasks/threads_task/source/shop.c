#include "threads.h"

#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define SHOP_NUMBER 5
#define SUPPLIER_NUMBER 1
#define CUSTOMER_NUMBER 3

static pthread_mutex_t shops[SHOP_NUMBER];
static long shop_stocks[SHOP_NUMBER];
static long demands[CUSTOMER_NUMBER];
static long supplier_caps[SUPPLIER_NUMBER];
static char customers_satisfied = 0;

void* customer_routine(void* arg) {
    int id = *((int*)(arg));
    while (demands[id] > 0) {
        size_t i = random() % SHOP_NUMBER;
        pthread_mutex_lock(&shops[i]);
        printf("customer %d entering shop %lu, demand = %ld, stock = %ld\n",
               id, i, demands[id], shop_stocks[i]);

        demands[id] -= shop_stocks[i];
        shop_stocks[i] = 0;
        printf("customer %d finished, new demand is %ld\n",
               id, demands[id]);

        pthread_mutex_unlock(&shops[i]);
    }
    printf("customer %d demand satisfied\n", id);
    return NULL;
}

void* supplier_routine(void* arg) {
    int id = *((int*)(arg));

    while (customers_satisfied == 0) {
        size_t shop_to_visit = random() % SHOP_NUMBER;
        pthread_mutex_lock(&shops[shop_to_visit]);
        printf("supplier entering shop %lu, cap is %ld\n",
               shop_to_visit, supplier_caps[id]);

        shop_stocks[shop_to_visit] += supplier_caps[id];
        supplier_caps[id] = random() % 1000;

        printf("finished supplying shop %lu, new cap is %lu, new stock is %lu\n",
               shop_to_visit, supplier_caps[id], shop_stocks[shop_to_visit]);
        pthread_mutex_unlock(&shops[shop_to_visit]);
    }

    return NULL;
}

void shop_task() {
    pthread_mutex_t src = PTHREAD_MUTEX_INITIALIZER;
    for (size_t i = 0; i < SHOP_NUMBER; ++i) {
        memmove(&(shops[i]), &src, sizeof(pthread_mutex_t));
        shop_stocks[i] = random() % 1000;
    }

    pthread_t thread_ids[CUSTOMER_NUMBER + SUPPLIER_NUMBER];
    int customer_ids[CUSTOMER_NUMBER];
    for (size_t i = 0; i < CUSTOMER_NUMBER; ++i) {
        customer_ids[i] = (int)i;

        int s = pthread_create(&(thread_ids[i]), NULL, customer_routine, &customer_ids[i]);
        if (s != 0) {
            perror("pthread_create error");
            exit(1);
        }
        demands[i] = random() % 10000;
    }

    int supplier_ids[SUPPLIER_NUMBER];
    for (size_t i = 0; i < SUPPLIER_NUMBER; ++i) {
        customer_ids[i] = (int)i;

        int s = pthread_create(&(thread_ids[i + CUSTOMER_NUMBER]), NULL, supplier_routine, &supplier_ids[i]);
        if (s != 0) {
            perror("pthread_create error");
            exit(1);
        }
        supplier_caps[i] = random() % 1000;
    }

    for (size_t i = 0; i < CUSTOMER_NUMBER + SUPPLIER_NUMBER; ++i) {
        void* cur_a = NULL;
        int s = pthread_join(thread_ids[i], &cur_a);
        if (s != 0) {
            perror("join error");
            exit(1);
        }
        if (i == CUSTOMER_NUMBER - 1) {
            customers_satisfied = 1;
        }
    }
    printf("\nall threads joined\n");

}








