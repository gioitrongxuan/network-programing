/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Server clients function file
*/

#include "../inc/server.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int genere_id(server_t *server)
{
    int used = 0;

    for (int id = 1; id <= MAX_CLIENTS; id++) {
        used = 0;
        for (int i = 0; i < server->actual_index; i++) {
            if (server->clients[i].index == id)
                used = 1;
        }
        if (!used)
            return (id);
    }
    return (0);
}

client_t create_client(server_t *server, sockaddr_in_t *csin)
{
    client_t client;

    client.index = genere_id(server);
    client.sin = *csin;
    server->clients[server->actual_index] = client;
    server->actual_index++;
    return (client);
}

int client_exists(server_t *server, sockaddr_in_t *csin)
{
    int i = 0;

    while (i < server->actual_index) {
        if (server->clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
            && server->clients[i].sin.sin_port == csin->sin_port)
            return (1);
        i++;
    }
    return (0);
}

client_t *getclient_t(server_t *server, sockaddr_in_t *csin)
{
    int i = 0;

    while (i < server->actual_index) {
        if (server->clients[i].sin.sin_addr.s_addr == csin->sin_addr.s_addr
            && server->clients[i].sin.sin_port == csin->sin_port)
            return (&(server->clients[i]));
        i++;
    }
    return (NULL);
}

void remove_client(server_t *server, int to_remove)
{
    memmove(server->clients + to_remove, server->clients + to_remove + 1
            , (server->actual_index - to_remove) * sizeof(client_t));
    server->actual_index--;
}
