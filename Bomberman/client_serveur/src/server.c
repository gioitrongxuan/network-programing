/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Server function file
*/

#include "../inc/action.h"
#include "../inc/app.h"
#include "../inc/move.h"
#include "../inc/server.h"
#include "../inc/state.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int server_event_keyboard(app_t *app, game_t *game, SDL_Event *e
        , server_t *server, client_t *sender)
{
    int result = STATE_SERVER_SOCKET;

    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
        result = STATE_SERVER;
        break;
    case SDLK_UP:
        if (server->actual_index > 1) {
            send_all_clients(server, sender, "up");
            move(app, game, 0, SDLK_UP);
        }
        break;
    case SDLK_DOWN:
        if (server->actual_index > 1) {
            send_all_clients(server, sender, "down");
            move(app, game, 0, SDLK_DOWN);
        }
        break;
    case SDLK_LEFT:
        if (server->actual_index > 1) {
            send_all_clients(server, sender, "left");
            move(app, game, 0, SDLK_LEFT);
        }
        break;
    case SDLK_RIGHT:
        if (server->actual_index > 1) {
            send_all_clients(server, sender, "right");
            move(app, game, 0, SDLK_RIGHT);
        }
        break;
    case SDLK_SPACE:
        if (server->actual_index > 1) {
            send_all_clients(server, sender, "bomb");
            action(game, 0);
        }
        break;
    default:
        break;
    }
    return (result);
}

static int server_event(app_t *app, game_t *game, server_t *server, client_t *sender)
{
    SDL_Event e;
    int result = STATE_SERVER_SOCKET;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            result = STATE_EXIT;
        else if (e.type == SDL_KEYDOWN) {
            result = server_event_keyboard(app, game, &e, server, sender);
        }
    }
    return (result);
}

static void server_waiting_draw(app_t *app, button_t *button)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    button_draw(app, button);
    SDL_RenderPresent(app->renderer);
}

static int init_connection(int port, sockaddr_in_t *sin)
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
    //
    printf("test\n");
    printf("\033[0;32m=> Server launched and listening on port %d and ip : %s.\033[0m\n", port , sin->sin_addr);
    return (socketfd);
}

static void server_interpet_message(app_t *app, game_t *game, char buffer[BUF_SIZE]
                                , int index, server_t *server)
{
    if (!strcmp(buffer, "join"))
        game->players[index - 1]->is_alive = 1;
    else if (!strcmp(buffer, "up"))
        move(app, game, index - 1, SDLK_UP);
    else if (!strcmp(buffer, "down"))
        move(app, game, index - 1, SDLK_DOWN);
    else if (!strcmp(buffer, "left"))
        move(app, game, index - 1, SDLK_LEFT);
    else if (!strcmp(buffer, "right"))
        move(app, game, index - 1, SDLK_RIGHT);
    else if (!strcmp(buffer, "bomb"))
        action(game, index - 1);
    else if (!strcmp(buffer, "exit")) {
        remove_client(server, index - 1);
        game->players[index - 1]->is_alive = 0;
        printf("Player %d left the game\n", index);
    }
}

static server_t *server_create(app_t *app)
{
    server_t *server = malloc(sizeof(server_t));
    SDL_Rect button_pos1 = { 7.5 * app->tile_size, 10 * app->tile_size
                            , 10 * app->tile_size, 1.1 * app->tile_size };

    if (server == NULL) {
        fprintf(stderr, "Failed to malloc server");
        return (NULL);
    }
    server->socketfd = init_connection(app->port, &(server->sin));
    server->actual_index = 0;
    server->max_index = server->socketfd;
    server->wait_text = button_create(app, "En attente de joueurs...", button_pos1);
    create_client(server, &(server->sin));
    return (server);
}

static void server_destroy(server_t *server)
{
    if (server != NULL) {
        if (server->wait_text != NULL)
            button_destroy(server->wait_text);
        closesocket(server->socketfd);
        free(server);
    }
}

int server_run(app_t *app)
{
    server_t *server = server_create(app);
    int state = STATE_SERVER_SOCKET;
    char buffer[BUF_SIZE];
    fd_set rdfs;
    game_t *game = game_create(app);
    struct timeval waitd = { 0, 0 };

    game->players[server->actual_index - 1]->is_alive = 1;
    while (state == STATE_SERVER_SOCKET) {
        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(server->socketfd, &rdfs);
        if (select(server->max_index + 1, &rdfs, NULL, NULL, &waitd) == -1) {
            perror("select()");
            exit(errno);
        }
        if (FD_ISSET(server->socketfd, &rdfs))
        {
            sockaddr_in_t csin;

            memset(&csin, 0, sizeof(csin));
            read_client(server->socketfd, &csin, buffer);
            if (client_exists(server, &csin)) {
                client_t *client = getclient_t(server, &csin);
                if (client != NULL) {
                    printf("%d %s\n", client->index, buffer);
                    send_all_clients(server, client, buffer);
                    server_interpet_message(app, game, buffer, client->index
                                            , server);
                }
            }
            else if (server->actual_index != MAX_CLIENTS) {
                client_t tmp_client = create_client(server, &csin);
                game->players[tmp_client.index - 1]->is_alive = 1;
                send_notif_join(server, &tmp_client);
                printf("Player %d joined the game\n", tmp_client.index);
            }
        }
        state = server_event(app, game, server, &(server->clients[0]));
        if (server->actual_index > 1)
            game_draw(app, game);
        else
            server_waiting_draw(app, server->wait_text);
        SDL_Delay(20);
    }
    game_destroy(game);
    server_destroy(server);
    return (state);
}
