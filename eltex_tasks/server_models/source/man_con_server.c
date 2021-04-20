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
#include <pthread.h>
#include <string.h>

typedef struct {
    comm_package_t order;
    struct sockaddr_in client_addr;
    int thread_working_flag;
    pthread_mutex_t mutex;
    pthread_t thread_id;

} thread_info_t;

void* thread_routine(void* _arg) {
    thread_info_t* arg = (thread_info_t*)_arg;
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket in thread routine");
        exit(1);
    }
    while(1) {
        if (arg->thread_working_flag == 0) {
            sleep(1);
        }
        else {
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
            arg->thread_working_flag = 0;
        }
    }
}

void man_con_server() {
    size_t thread_count = 10, free_threads_count = 10, occupied_threads_count = 0;
    thread_info_t* threads = malloc(thread_count * sizeof(thread_info_t));
    thread_info_t** free_threads = malloc(thread_count * sizeof(thread_info_t*));
    thread_info_t** occupied_threads = calloc(thread_count, sizeof(thread_info_t**));
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        threads[i].thread_working_flag = 0;

        if (pthread_create(&threads[i].thread_id, &thread_attr, thread_routine, threads + i) < 0) {
            perror("pthread create");
            exit(1);
        }
        pthread_attr_destroy(&thread_attr);
        free_threads[i] = threads + i;
    }

    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(MANCONSERV_UDP_PORT); // a port that is hopfully free
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
        if (free_threads_count > 0) {
            // if no free threads just ignore request, whatever
            thread_info_t* thread = free_threads[free_threads_count - 1];
            --free_threads_count;
            memcpy(&thread->order, &buf, sizeof(thread->order));
            memcpy(&thread->client_addr, &client_addr, sizeof(thread->client_addr));
            thread->thread_working_flag = 1;
            occupied_threads[occupied_threads_count] = thread;
            ++occupied_threads_count;
        }
        thread_info_t** temp = calloc(occupied_threads_count, sizeof(thread_info_t));
        size_t new_occupied_threads_count = 0;
        for (size_t i = 0; i < occupied_threads_count; ++i) {
            if (occupied_threads[i]->thread_working_flag == 0) {
                free_threads[free_threads_count] = occupied_threads[i];
                ++free_threads_count;
            }
            else {
                temp[new_occupied_threads_count] = occupied_threads[i];
                ++new_occupied_threads_count;

            }
        }
        memcpy(occupied_threads, temp, new_occupied_threads_count);
        occupied_threads_count = new_occupied_threads_count;
        free(temp);
    }
    free(threads);
    free(free_threads);
    free(occupied_threads);
}
