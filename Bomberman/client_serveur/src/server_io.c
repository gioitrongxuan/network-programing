/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Server i/o function file
*/

#include "../inc/server.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_client(int socketfd, sockaddr_in_t *csin, char *buffer)
{
    int n = 0;
    socklen_t sinsize = sizeof(*csin);

    if ((n = recvfrom(socketfd, buffer, BUF_SIZE - 1, 0, (sockaddr_t *)csin, &sinsize)) < 0) {
        perror("recvfrom()");
        exit(errno);
    }
    buffer[n] = '\0';
    return n;
}

static void write_client(int socketfd, sockaddr_in_t *sin, const char *buffer)
{
    if (sendto(socketfd, buffer, strlen(buffer), 0, (sockaddr_t *)sin, sizeof(*sin)) < 0) {
        perror("sendto()");
        exit(errno);
    }
}

void send_notif_join(server_t *server, client_t *sender)
{
    int i = 1;
    char index_string[2] = { 0 };
    char message[BUF_SIZE] = { 0 };

    sprintf(index_string, "%d", sender->index);
    strncpy(message, index_string, BUF_SIZE - 1);
    strncat(message, " join", sizeof(message) - strlen(message) - 1);
    while (i < server->actual_index) {
        write_client(server->socketfd, &(server->clients[i].sin), message);
        i++;
    }
}


void send_all_clients(server_t *server, client_t *sender, const char *buffer)
{
    int i = 1;
    char index_string[2] = { 0 };
    char message[BUF_SIZE] = { 0 };

    sprintf(index_string, "%d", sender->index);
    strncpy(message, index_string, BUF_SIZE - 1);
    strncat(message, " ", sizeof(message) - strlen(message) - 1);
    strncat(message, buffer, sizeof(message) - strlen(message) - 1);
    while (i < server->actual_index) {
        if (sender != &(server->clients[i]))
            write_client(server->socketfd, &(server->clients[i].sin), message);
        i++;
    }
}
