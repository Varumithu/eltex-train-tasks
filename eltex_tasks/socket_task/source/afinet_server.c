#include "sockets_task.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>



void afinet_server() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(AFINET_SERVER_PORT); // a port that is hopfully free



    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }
    int s = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (s != 0) {
        perror("bind");
        exit(1);
    }

    s = listen(socket_fd, 5);
    if (s != 0) {
        perror("listen");
        exit(1);
    }

    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int active_socket_fd = accept(socket_fd,
                                  (struct sockaddr*)&client_addr, &addrlen);
    if (active_socket_fd < 0) {
        perror("accept");
        exit(1);
    }
    char buf[100] = "this is server sending to client";
    send(active_socket_fd, buf, strlen(buf) + 1, 0);
    recv(active_socket_fd, buf, 100, 0);
    printf("%s\n", buf);
}
