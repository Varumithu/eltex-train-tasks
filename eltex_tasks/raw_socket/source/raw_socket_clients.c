#include "raw_sockets.h"
#include "server_models.h"

#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>

void init_message(raw_message_t* p_message) {
    p_message->header.length = htons(sizeof(raw_message_t));
    p_message->header.checksum = 0;
    p_message->header.source_port = 0;
    p_message->header.dest_port = 0;
    p_message->data.info = 0;
}

void raw_udp_client() {
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
//    serv_addr.sin_port = htons(SELSERV_UDP_PORT);
    serv_addr.sin_port = 0;//

    raw_message_t message;
    init_message(&message);
    message.header.source_port = htons(RAW_SOCKET_UDP_PORT);
    message.header.dest_port = htons(SELSERV_UDP_PORT);

    while(1){
        printf("enter buf value\n");
        scanf("%d", &message.data.info);
        raw_message_t received = {{0, 0, 0, 0}, {0}};
        sendto(socket_fd, &message, sizeof(message), 0,
               (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));

        while(1) {
            recvfrom(socket_fd, &received, sizeof(raw_message_t), 0,
                     NULL, NULL);
            printf("received a packet from port %d to port %d\n",
                   ntohs(received.header.source_port),
                   ntohs(received.header.dest_port));
            if (received.header.source_port == htons(SELSERV_UDP_PORT) ||
                received.header.dest_port == htons(RAW_SOCKET_UDP_PORT)) {
                printf("received %d\n", message.data.info);
                break;
            }
        }
        if (received.data.info == 0) {
            break;
        }
    }

}
