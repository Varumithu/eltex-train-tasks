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
} raw_message_t; // old struct for only udp header filling

typedef struct {
    uint8_t version_and_ihl;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_and_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source_addr;
    uint32_t dest_addr;
} ip_noopt_header_t;

typedef struct {
    ip_noopt_header_t ip_header;
    udp_header_t udp_header;
    comm_package_t data;
} ip_noopt_udp_t;

void raw_udp_client();

void raw_ip_udp_client();

#endif
