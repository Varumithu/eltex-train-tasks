#include "sockets_task.h"

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>



void aflocal_server() {
    // in case it was created and not deleted by previous launch
    unlink("./exltex_tasks_socket_task_aflocal_server");
    struct sockaddr_un addr = {AF_LOCAL, "./exltex_tasks_socket_task_aflocal_server"};

    int socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket creation");
        exit(1);
    }

    int s = bind(socket_fd, (struct sockaddr*)(&addr), sizeof(addr));
    if (s != 0) {
        perror("bind");
        exit(1);
    }

    s = listen(socket_fd, 5);
    if (s != 0) {
        perror("listen");
        exit(1);
    }

    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int active_socket_fd = accept(socket_fd,
                                  (struct sockaddr*)(&client_addr), &client_addr_len);
    if (active_socket_fd < 0) {
        perror("accept");
        exit(1);
    }
    if (client_addr_len > sizeof(client_addr)) {
        printf("i am not ready for this\n");
        exit(1);
    }

    char buf[100] = "hi i'm server";
    send(active_socket_fd, buf, strlen(buf) + 1, 0);
    recv(active_socket_fd, buf, 100, 0);
    printf("%s\n", buf);

    unlink("./exltex_tasks_socket_task_aflocal_server");
}
