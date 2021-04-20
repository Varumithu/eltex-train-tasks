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

void test_selecting_server() {
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket");
        exit(1);
    }


    struct sockaddr_in tcp_addr, udp_addr;

    tcp_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &tcp_addr.sin_addr);
    tcp_addr.sin_port = htons(SELSERV_TCP_PORT); // a port that is hopfully free

    udp_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &udp_addr.sin_addr);
    udp_addr.sin_port = htons(SELSERV_UDP_PORT); // a port that is hopfully free
    socklen_t addrlen = sizeof(struct sockaddr_in);
    comm_package_t buf = {0}, sendbuf = {0};

    while(1) {
        int user_input = 0;

        printf("1 for tcp message, 2 for udp\nthen enter buf value\n");
        scanf("%d%d%*c", &user_input, &sendbuf.info);
        if (user_input == 1) {
            int tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (tcp_socket_fd < 0) {
                perror("tcp socket");
                exit(1);
            }


            if (connect(tcp_socket_fd, (struct sockaddr*)&tcp_addr, addrlen) < 0) {
                perror("tcp connect");
                exit(1);
            }
            if (send(tcp_socket_fd, &sendbuf, sizeof(buf), 0) < 0) {
                perror("tcp send");
                exit(1);
            }
            if (recv(tcp_socket_fd, &buf, sizeof(buf), 0) < 0) {
                perror("tcp receive");
                exit(1);
            }
            printf("received %d\n", buf.info);
            if (sendbuf.info == -1) {
                break;
            }
            shutdown(tcp_socket_fd, SHUT_RDWR);
            close(tcp_socket_fd);
            continue;

        }
        else if (user_input == 2){
            sendto(udp_socket_fd, &sendbuf, sizeof(sendbuf), 0,
                   (struct sockaddr*)&udp_addr, addrlen);
            recvfrom(udp_socket_fd, &buf, sizeof(sendbuf), 0,
                     (struct sockaddr*)&udp_addr, &addrlen);
            printf("received %d\n", buf.info);
            if (sendbuf.info == -1) {
                break;
            }
            continue;
        }

    }
}

void test_man_con_server() {

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(MANCONSERV_UDP_PORT); // a port that is hopfully free

    socklen_t addrlen = sizeof(struct sockaddr_in);
    comm_package_t buf = {0}, sendbuf = {0};
    char loop_stop = 0;
    while(loop_stop == 0) {

        int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket_fd < 0) {
            perror("udp socket");
            exit(1);
        }


        size_t request_number = 0;
        printf("how many requests to send?\n");
//        comm_package_t* requests = malloc(request_number * sizeof(comm_package_t));
//        if (requests == NULL) {
//            perror("malloc");
//            exit(1);
//        }

        scanf("%lu", &request_number);
        printf("how long to sleep before trying to receive replies?\n");
        unsigned int seconds = 0;
        scanf("%du", &seconds);

        for (size_t i = 0; i < request_number; ++i) {
            printf("enter value for request %lu\n", i);
            scanf("%d", &sendbuf.info);
            sendto(udp_socket_fd, &sendbuf, sizeof(comm_package_t), 0,
                   (struct sockaddr*)&addr, addrlen);
            if (sendbuf.info == -1) {
                loop_stop = 1;
            }
        }
        sleep(seconds);
        size_t count = 0;
        while(recvfrom(udp_socket_fd, &buf, sizeof(comm_package_t), MSG_DONTWAIT,
                       NULL, NULL) > 0) {
            printf("received reply number %lu, value is %d\n", count, buf.info);
            ++count;
        }
        close(udp_socket_fd);
//        free(requests);
    }
}

void test_creating_server() {
    // this is temporary, i think this should be different
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(THREADCREATING_UDP_PORT); // a port that is hopfully free

    socklen_t addrlen = sizeof(struct sockaddr_in);
    comm_package_t buf = {0}, sendbuf = {0};
    char loop_stop = 0;
    while(loop_stop == 0) {

        int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_socket_fd < 0) {
            perror("udp socket");
            exit(1);
        }


        size_t request_number = 0;
        printf("how many requests to send?\n");
//        comm_package_t* requests = malloc(request_number * sizeof(comm_package_t));
//        if (requests == NULL) {
//            perror("malloc");
//            exit(1);
//        }

        scanf("%lu", &request_number);
        printf("how long to sleep before trying to receive replies?\n");
        unsigned int seconds = 0;
        scanf("%du", &seconds);

        for (size_t i = 0; i < request_number; ++i) {
            printf("enter value for request %lu\n", i);
            scanf("%d", &sendbuf.info);
            sendto(udp_socket_fd, &sendbuf, sizeof(comm_package_t), 0,
                   (struct sockaddr*)&addr, addrlen);
            if (sendbuf.info == -1) {
                loop_stop = 1;
            }
        }
        sleep(seconds);
        size_t count = 0;
        while(recvfrom(udp_socket_fd, &buf, sizeof(comm_package_t), MSG_DONTWAIT,
                       NULL, NULL) > 0) {
            printf("received reply number %lu, value is %d\n", count, buf.info);
            ++count;
        }
        close(udp_socket_fd);
//        free(requests);
    }
}











