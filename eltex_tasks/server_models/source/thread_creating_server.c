#define _GNU_SOURCE
#include "server_models.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    comm_package_t order;
    struct sockaddr_in client_addr;
    int thread_working_flag;
    pthread_mutex_t mutex;
    pthread_t thread_id;

} thread_info_t;

struct thread_list_node {
    thread_info_t* thread_info;
    struct thread_list_node* next_node;
    struct thread_list_node* prev_node;
};

typedef struct thread_list_node thread_list_node_t;

thread_list_node_t* thread_list_add(thread_list_node_t* where, thread_info_t* info) {
    thread_list_node_t* new = malloc(sizeof(thread_list_node_t));
    if (new == NULL) {
        perror("malloc in thread_list_add");
        exit(1);
    }
    new->thread_info = malloc(sizeof(thread_info_t));
    if (new->thread_info == NULL) {
        perror("malloc in thread_list_add");
        exit(1);
    }
    memcpy(new->thread_info, info, sizeof(thread_info_t));
    new->prev_node = where;
    if (where != NULL) {
        new->next_node = where->next_node;
        where->next_node = new;
    }

    return new;
}

void thread_list_delete(thread_list_node_t* arg) {
    if (arg->prev_node != NULL) {
        arg->prev_node->next_node = arg->next_node;
    }
    if (arg->next_node != NULL) {
        arg->next_node->prev_node = arg->prev_node;
    }
    free(arg->thread_info);
    free(arg);
}

void* thread_routine_cs(void* _arg) {
    thread_info_t* arg = (thread_info_t*)_arg;
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket in thread routine");
        exit(1);
    }
    //doing some useless stuff that takes a while
    size_t a = 0;
    for (size_t i = 0; i < 99999999; ++i) {
        if (i % 2 == 0) {
            ++a;
        }
        else {
            --a;
        }
    }
    arg->order.info += a;
    if (sendto(udp_socket_fd, &arg->order, sizeof(comm_package_t), 0,
               (struct sockaddr*)&arg->client_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("sendto");
        exit(1);
    }

    return _arg;
}

void thread_creating() {
    size_t thread_max = 10, thread_cur = 0;
    thread_list_node_t* thread_list_last = NULL;
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(THREADCREATING_UDP_PORT); // a port that is hopfully free
    socklen_t addrlen = sizeof(addr);
    if (bind(udp_socket_fd, (struct sockaddr*)&addr, addrlen) < 0) {
        perror("bind");
        exit(1);
    }
    struct sockaddr_in client_addr;
    comm_package_t buf = {0};
    while(1) {
        if (recvfrom(udp_socket_fd, &buf, sizeof(buf), 0,
                     (struct sockaddr*)&client_addr, &addrlen) < 0) {
            perror("recvfrom");
            exit(1);
        }
        if (buf.info == -1) {
            break;
        }
        if (thread_cur <= thread_max) {
            //if too much threads just ignore the request, whatever
            ++thread_cur;
            thread_info_t info;
            memcpy(&info.client_addr, &client_addr, sizeof(client_addr));
            info.order.info = buf.info;
            thread_list_last = thread_list_add(thread_list_last, &info);
            pthread_create(&thread_list_last->thread_info->thread_id,
                           NULL, thread_routine_cs, thread_list_last->thread_info);

        }
        for (thread_list_node_t* iterator = thread_list_last; iterator != NULL;
             iterator = iterator->prev_node) {
            void* _retval = NULL;
            if (pthread_tryjoin_np(iterator->thread_info->thread_id, &_retval) != 0)
                continue;

//            thread_info_t* retval = (thread_info_t*)_retval;
            thread_list_node_t* temp = iterator;
            iterator = iterator->prev_node;
            thread_list_delete(temp);
            if (iterator == NULL) {
                thread_list_last = iterator;
                break;
            }
            if (iterator->next_node == NULL) {
                thread_list_last = iterator;
            }
        }

    }

}
