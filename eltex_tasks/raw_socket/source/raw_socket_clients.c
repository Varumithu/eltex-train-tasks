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
    size_t ip_header_max = 60;

    char* received_full = malloc(ip_header_max + sizeof(raw_message_t));
    memset(received_full, 0, ip_header_max + sizeof(raw_message_t));


    while(1){
        printf("enter buf value\n");
        scanf("%d", &message.data.info);
        sendto(socket_fd, &message, sizeof(message), 0,
               (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));
        raw_message_t* received_udp = NULL;
        while(1) {
            recvfrom(socket_fd, received_full, sizeof(raw_message_t) + ip_header_max, 0,
                     NULL, NULL);
            uint8_t ihl = *((uint8_t*)received_full) & 0x0f;
            ihl = ihl * 4; //length in bytes
            received_udp = (raw_message_t*)(received_full + ihl);
            printf("received a packet from port %d to port %d\n",
                   ntohs(received_udp->header.source_port),
                   ntohs(received_udp->header.dest_port));
            if (received_udp->header.source_port == htons(SELSERV_UDP_PORT) ||
                received_udp->header.dest_port == htons(RAW_SOCKET_UDP_PORT)) {
                printf("received %d\n", received_udp->data.info);
                break;
            }
        }
        if (received_udp->data.info == 0) {
            break;
        }
    }
    free(received_full);

}

void init_message_with_ipheader(ip_noopt_udp_t* message) {
    message->ip_header.version_and_ihl = 0x45;
    message->ip_header.type_of_service = 0; //hopefully it's okay
    message->ip_header.total_length = htons(sizeof(ip_noopt_udp_t));
    message->ip_header.identification = 0;
    message->ip_header.flags_and_fragment_offset = 0;
    message->ip_header.ttl = 100;
    message->ip_header.protocol = IPPROTO_UDP;
    message->ip_header.checksum = 0;
    message->udp_header.length = htons(sizeof(udp_header_t) + sizeof(comm_package_t));
    message->udp_header.checksum = 0;
    message->udp_header.dest_port = 0;
    message->udp_header.source_port = 0;
    message->data.info = 0;
}

void raw_ip_udp_client() {
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }
    int option = 1;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, &option, sizeof(int)) < 0 ) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
//    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_addr.s_addr = 0; //maybe?
//    serv_addr.sin_port = htons(SELSERV_UDP_PORT);
    serv_addr.sin_port = 0;//

    size_t ip_header_max = 60;

    ip_noopt_udp_t message;
    init_message_with_ipheader(&message);
    inet_aton("127.0.0.1", (struct in_addr*)&message.ip_header.source_addr);
    inet_aton("127.0.0.1", (struct in_addr*)&message.ip_header.dest_addr);
    message.udp_header.dest_port = htons(SELSERV_UDP_PORT);
    message.udp_header.source_port = htons(RAW_SOCKET_UDP_PORT);

    char* received_full = malloc(sizeof(ip_noopt_udp_t));
    memset(received_full, 0, sizeof(ip_noopt_udp_t));


    while(1){
        printf("enter buf value\n");
        scanf("%d", &message.data.info);
        if (sendto(socket_fd, &message, sizeof(message), 0,
                   (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)) < 0) {
            perror("sendto");
            continue;
        }

//        write(socket_fd, &message, sizeof(message));
//        raw_message_t* received_udp = NULL;
        ip_noopt_udp_t* received_ip_udp = NULL;
        while(1) {
            recvfrom(socket_fd, received_full, sizeof(raw_message_t) + ip_header_max, 0,
                     NULL, 0);
            uint8_t ihl = *((uint8_t*)received_full) & 0x0f;
            if (ihl > 5) {
                printf("can't work with long ip headers for now\n");
                continue;
            }
            ihl = ihl * 4; //length in bytes
            received_ip_udp = (ip_noopt_udp_t*)(received_full);
            struct in_addr source = {received_ip_udp->ip_header.source_addr},
                           dest = {received_ip_udp->ip_header.dest_addr};
            printf("received a packet from ip %s to ip %s\n",
                   inet_ntoa(source), inet_ntoa(dest));
            printf("from port %d to port %d\n",
                   ntohs(received_ip_udp->udp_header.source_port),
                   ntohs(received_ip_udp->udp_header.dest_port));
            if (received_ip_udp->udp_header.source_port == htons(SELSERV_UDP_PORT) ||
                received_ip_udp->udp_header.dest_port == htons(RAW_SOCKET_UDP_PORT)) {
                printf("received %d\n", received_ip_udp->data.info);
                break;
            }
        }
        if (received_ip_udp->data.info == 0) {
            break;
        }
    }
    free(received_full);
}
