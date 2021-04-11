#include "sockets_task.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>




void aflocal_client() {
    struct sockaddr_un serv_addr = {AF_LOCAL, "./exltex_tasks_socket_task_aflocal_server"};
    int socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket creation");
        exit(1);
    }

    int s = connect(socket_fd, (struct sockaddr*)(&serv_addr), sizeof (serv_addr));
    if (s < 0) {
        perror("connect");
        exit(1);
    }

    char buf[100] = "hi i'm client";
    send(socket_fd, buf, strlen(buf) + 1, 0);
    recv(socket_fd, buf, 100, 0);
    printf("%s\n", buf);
}
