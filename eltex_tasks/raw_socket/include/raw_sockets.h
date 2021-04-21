#ifndef _ELTEX_TASKS_RAW_SOCKET_RAW_SOCKETS_H
#define _ELTEX_TASKS_RAW_SOCKET_RAW_SOCKETS_H

#include <stdint.h>

#include "server_models.h"

#define RAW_SOCKET_UDP_PORT 38399

typedef struct {
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} udp_header_t;

typedef struct {
    udp_header_t header;
    comm_package_t data;
} raw_message_t;

void raw_udp_client();

#endif
