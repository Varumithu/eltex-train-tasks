#include "sockets_task.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

void afinet_client() {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(AFINET_SERVER_PORT); // a port that is hopfully free

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }

    int s = connect(socket_fd, (struct sockaddr*)(&serv_addr), sizeof (serv_addr));
    if (s < 0) {
        perror("connect");
        exit(1);
    }


    char buf[100] = "this is client sending to server";
    send(socket_fd, buf, strlen(buf) + 1, 0);
    recv(socket_fd, buf, 100, 0);
    printf("%s\n", buf);
}
