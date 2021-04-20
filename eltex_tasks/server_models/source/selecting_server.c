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
#include <poll.h>

void selecting_server() {
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0) {
        perror("udp socket");
        exit(1);
    }
    int tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket_fd < 0) {
        perror("tcp socket");
        exit(1);
    }

    struct sockaddr_in tcp_addr, udp_addr;

    tcp_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &tcp_addr.sin_addr);
    tcp_addr.sin_port = htons(SELSERV_TCP_PORT); // a port that is hopfully free

    udp_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &udp_addr.sin_addr);
    udp_addr.sin_port = htons(SELSERV_UDP_PORT); // a port that is hopfully free

    if (bind(tcp_socket_fd, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) < 0) {
        perror("bind tcp");
        exit(1);
    }
    if (bind(udp_socket_fd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("bind udp");
        exit(1);
    }

    if (listen(tcp_socket_fd, 5) < 0) {
        perror("listen tcp");
        exit(1);
    }

    struct pollfd fds[2] = {{0, 0, 0}, {0, 0, 0}};
    fds[0].fd = tcp_socket_fd;
    fds[0].events |= POLLIN;

    fds[1].fd = udp_socket_fd;
    fds[1].events |= POLLIN;

    comm_package_t buf = {0}, sendbuf = {0};
    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));

    while (1) {
        if (poll(fds, 2, -1) < 0) {
            perror("poll");
            exit(1);
        }
        if ((fds[0].revents & POLLIN) == POLLIN) {
            //tcp socket
            int active_socket_fd = accept(tcp_socket_fd, (struct sockaddr*)&client_address,
                                          &addrlen);
            if (active_socket_fd < 0) {
                perror("accept");
                exit(1);
            }
            recv(active_socket_fd, &buf, sizeof(buf), 0);
            sendbuf.info = buf.info + 1;
            send(active_socket_fd, &sendbuf, sizeof(sendbuf), 0);
            shutdown(active_socket_fd, SHUT_RDWR);
        }
        if (buf.info == -1) {
            break;
        }
        if ((fds[1].revents & POLLIN) == POLLIN) {
            //udp
            recvfrom(udp_socket_fd, &buf, sizeof(buf),
                     0, (struct sockaddr*)&client_address, &addrlen);
            sendbuf.info = buf.info + 1;
            sendto(udp_socket_fd, &sendbuf, sizeof(sendbuf), 0,
                   (struct sockaddr*)&client_address, addrlen);
        }
        if (buf.info == -1) {
            break;
        }
    }
}




