#include "chat.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <malloc.h>

typedef struct {
    size_t id;
    char username[100];
    mqd_t to_client_mq;
} client_t;

typedef struct {
    char* text;
    size_t client_id;
    size_t id;
} message_t;

void add_message(size_t* iter, size_t buf_size, message_t* message_buf,
                 char* text, client_t* p_client, size_t message_id)
{
    if (message_buf[*iter].text != NULL) {
        free(message_buf[*iter].text);
        message_buf[*iter].text = NULL;
    }
    message_buf[*iter].text = malloc(strlen(text) + 1 + strlen(p_client->username) + 2);
    strcpy(message_buf[*iter].text, p_client->username);
    strcat(message_buf[*iter].text, ": ");
    strcat(message_buf[*iter].text, text);
    message_buf[*iter].client_id = p_client->id;
    message_buf[*iter].id = message_id;
    *iter = ++(*iter) % buf_size;
}

client_t* find_user_by_name(char* name, client_t* clients, size_t client_count) {
    for (size_t i = 0; i < client_count; ++i) {
        if (strcmp(clients[i].username, name) == 0) {
            return  clients + i;
        }
    }
    return NULL;
}

long send_updates(client_t* clients, size_t client_count, long last_update_message_id,
                    message_t* message_buf, size_t message_buf_size, size_t message_buf_iter) {

    long buf_update_start_ptr = 0;
    if (message_buf[message_buf_iter].text == NULL) {
        buf_update_start_ptr = last_update_message_id + 1;
    }
    else {
        char found = 0;
        for (size_t i = message_buf_iter; i < message_buf_size; ++ i) {
            if (message_buf[i].id == (size_t)last_update_message_id) {
                buf_update_start_ptr = (long)((i + 1) % message_buf_size);
                found = 1;
                break;
            }
        }
        if (found == 0){
            for (size_t i = 0; i < message_buf_iter; ++ i) {
                if ((long)message_buf[i].id == last_update_message_id) {
                    buf_update_start_ptr = (long)((i + 1) % message_buf_size);
                    found = 1;
                    break;
                }
            }
        }
    }
    size_t messages_to_send_count = 0;
    if (buf_update_start_ptr < (long)message_buf_iter) {
        messages_to_send_count = message_buf_iter - (size_t)buf_update_start_ptr;
    }
    else if ((size_t)buf_update_start_ptr > message_buf_iter) {
        messages_to_send_count = message_buf_size -
                (size_t)buf_update_start_ptr + message_buf_iter;
    }
    else {
        messages_to_send_count = message_buf_size;
    }
    message_t* messages_to_send = malloc(messages_to_send_count);
    if (messages_to_send == NULL) {
        perror("failed to allocate messages to send");
        exit(1);
    }
    if (buf_update_start_ptr < (long)message_buf_iter) {
        for (size_t i = 0; (size_t)buf_update_start_ptr + i < message_buf_iter; ++i) {
            messages_to_send[i] = message_buf[(size_t)buf_update_start_ptr + i];
        }
    }
    else if ((size_t)buf_update_start_ptr >= message_buf_iter) {
        for (size_t i = 0; (size_t)buf_update_start_ptr + i < message_buf_size; ++i) {
            messages_to_send[i] = message_buf[(size_t)buf_update_start_ptr + i];
        }
        for (size_t i = 0; i < message_buf_iter; ++i) {
            messages_to_send[i + message_buf_size - (size_t)buf_update_start_ptr] =
                    message_buf[i];
        }
    }
    last_update_message_id = (long)messages_to_send[messages_to_send_count - 1].id;
    for (size_t i = 0; i < client_count; ++i) {
        for (size_t j = 0; j < messages_to_send_count; ++j) {
            int ret = mq_send(clients[i].to_client_mq, messages_to_send[j].text,
                    strlen(messages_to_send[j].text) + 1, 1);
            if (ret < 0) {
                perror("sending message to client");
                exit(1);
            }
            printf("message %s sent to client %lu %s\n", messages_to_send[j].text,
                   clients[i].id, clients[i].username);

        }
    }
    return last_update_message_id;
}

void chat_server() {
    mqd_t connection_mq = mq_open("/chat_server_connection",
                       O_RDWR | O_CREAT,
                       S_IRUSR | S_IWUSR |
                       S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, NULL);

    if (connection_mq == (mqd_t)-1) {
        perror("failed to create queue");
        exit(1);
    }
    printf("connection queue created\n");
    struct mq_attr attr;
    mq_getattr(connection_mq, &attr);
    char* buf = malloc((unsigned long)attr.mq_msgsize);
    if (buf == NULL) {
        perror("failed to allocate message text buffer");
        exit(1);
    }

    size_t clients_allocated = 10;
    size_t message_buf_size = 10;
    size_t message_buf_iter = 0; // the location to write next message in the circular buffer
    message_t* message_buf = calloc(message_buf_size, sizeof(message_t));
    if (message_buf == NULL) {
        perror("failed to allocate circular message buf");
        exit(1);
    }

    client_t* clients = malloc(clients_allocated * sizeof (client_t));
    size_t client_count = 0;
    size_t message_count = 0;
    long last_update_message_id = -1;

    while (1) {
        unsigned int prio = 0;
        mq_receive(connection_mq, buf, (size_t)attr.mq_msgsize, &prio);
        if (prio == 2) {
            //new connection
            printf("incoming connection\n");
            ++client_count;
            if (client_count > clients_allocated) {
                // TODO
            }
            clients[client_count - 1].id = client_count - 1;
            strncpy(clients[client_count - 1].username, buf, 100);
            clients[client_count - 1].username[99] = '\0';
            strcpy(buf, "/");
            strcat(buf, clients[client_count - 1].username);
            clients[client_count - 1].to_client_mq = mq_open(buf, O_RDWR);
            printf("created client %s\n", clients[client_count - 1].username);
            if (clients[client_count - 1].to_client_mq == (mqd_t)-1) {
                perror("client queue opening");
                exit(1);
            }


        }
        if (prio == 3) {
            mq_unlink("/chat_server_connection");
            mq_close(connection_mq);
            break;
            // TODO some more code to properly turn off here, maybe send messages to other clients.
        }
        if (prio == 1) {
            //message received
            char* text_start = strchr(buf, ':') + 1;
            size_t name_length = (size_t)labs(text_start - buf) - 1;
            char* name = malloc(name_length);
            strncpy(name, buf, name_length);
            add_message(&message_buf_iter, message_buf_size,
                        message_buf, buf,
                        find_user_by_name(name, clients, client_count),
                        message_count);
            ++message_count;
            last_update_message_id = send_updates(clients, client_count, last_update_message_id,
                                                  message_buf, message_buf_size,
                                                  message_buf_iter);
            free(name);
        }
    }

    for (size_t i = 0; i < client_count; ++i) {
        mq_close(clients[i].to_client_mq);
    }

    free(clients);
    free(buf);
}



















