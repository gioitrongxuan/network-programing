/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Server i/o function file
*/

#include "../inc/server.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int server_connection(int port, sockaddr_in_t *sin)
{
    int socketfd;

    memset(sin, 0, sizeof(*sin));
    if ((socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
        perror("socket()");
        exit(errno);
    }
    printf("\033[0;32m=> Socket has been created.\033[0m\n");
    printf("\033[0;33m=> Attempting to start server...\033[0m\n");
    sin->sin_addr.s_addr = htonl(INADDR_ANY);
    sin->sin_port = htons(port);
    sin->sin_family = AF_INET;
    if (bind(socketfd, (sockaddr_t *) sin, sizeof(*sin)) == SOCKET_ERROR) {
        perror("bind()");
        exit(errno);
    }
    printf("\033[0;32m=> Server launched and listening on port %d.[0m\033\n", port);
    return (socketfd);
}

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
    if (sendto(socketfd, buffer, strlen(buffer), 0, (sockaddr_t *)sin
                                                    , sizeof(*sin)) < 0) {
        perror("sendto()");
        exit(errno);
    }
}

void send_notif_join(server_t *server, client_t *sender, app_t *app)
{
    char message[BUF_SIZE] = { 0 };

    encode_first_info(app, message);
    write_client(server->socketfd, &(sender->sin), message);
}


void send_all_clients(server_t *server, game_t *game)
{
    char message[BUF_SIZE] = { 0 };

    encode_game(game, message);
    for (int i = 1; i < server->actual_index; i++) {
        write_client(server->socketfd, &(server->clients[i].sin), message);
    }
}
