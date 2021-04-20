#ifndef _ELTEX_TASKS_SERVER_MODELS_SERVER_MODELS_H
#define _ELTEX_TASKS_SERVER_MODELS_SERVER_MODELS_H

#define SELSERV_TCP_PORT 38395
#define SELSERV_UDP_PORT 38396
#define MANCONSERV_UDP_PORT 38397
#define THREADCREATING_UDP_PORT 38398

typedef struct {
    int info;
} comm_package_t;

void selecting_server();

void test_selecting_server();

void man_con_server();

void test_man_con_server();

void thread_creating();

void test_creating_server();

#endif
