/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Server defintion file
*/

#ifndef _SERVER_H_
#define _SERVER_H_

#include "app.h"
#include "game.h"
#include "button.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define  BUF_SIZE            1024
#define  MAX_CLIENTS 	      4
#define  INVALID_SOCKET      -1
#define  SOCKET_ERROR        -1
#define  closesocket(s)      close(s)

typedef   struct sockaddr_in  sockaddr_in_t;
typedef   struct sockaddr     sockaddr_t;
typedef   struct in_addr      in_addr_bis_t;

typedef   struct              client_t {
    sockaddr_in_t             sin;
    int                       index;
}                             client_t;

typedef   struct              server_s {
    int                       socketfd;
    int                       actual_index;
    int                       max_index;
    client_t                  clients[MAX_CLIENTS];
    button_t                  *wait_text;
    sockaddr_in_t             sin;
}                             server_t;

client_t  create_client(server_t *server, sockaddr_in_t *csin);
int       read_client(int socketfd, sockaddr_in_t *csin, char *buffer);
int       client_exists(server_t *server, sockaddr_in_t *csin);
int       server_run(app_t *app);
void      send_notif_join(server_t *server, client_t *sender);
void      send_all_clients(server_t *server,  client_t *sender, const char *buffer);
void      remove_client(server_t *server, int to_remove);
client_t *getclient_t(server_t *server, sockaddr_in_t *csin);

#endif /* _SERVER_H_ */
