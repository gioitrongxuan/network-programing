/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Client function file
*/

#include "../inc/action.h"
#include "../inc/app.h"
#include "../inc/client.h"
#include "../inc/move.h"
#include "../inc/state.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int client_event_keyboard(app_t *app, game_t *game, SDL_Event *e
                                , client_helper_t *client)
{
    int result = STATE_CLIENT_SOCKET;

    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
        if (client->is_connected)
            write_server(client->socketfd, &(client->sin), "exit");
        result = STATE_CLIENT;
        break;
    case SDLK_UP:
        if (client->is_connected) {
            write_server(client->socketfd, &(client->sin), "up");
            move(app, game, client->id - 1, SDLK_UP);
        }
        break;
    case SDLK_DOWN:
        if (client->is_connected) {
            write_server(client->socketfd, &(client->sin), "down");
            move(app, game, client->id - 1, SDLK_DOWN);
        }
        break;
    case SDLK_LEFT:
        if (client->is_connected) {
            write_server(client->socketfd, &(client->sin), "left");
            move(app, game, client->id - 1, SDLK_LEFT);
        }
        break;
    case SDLK_RIGHT:
        if (client->is_connected) {
            write_server(client->socketfd, &(client->sin), "right");
            move(app, game, client->id - 1, SDLK_RIGHT);
        }
        break;
    case SDLK_SPACE:
        if (client->is_connected) {
            write_server(client->socketfd, &(client->sin), "bomb");
            action(game, client->id - 1);
        }
        break;
    default:
        break;
    }
    return (result);
}

static int client_event(app_t *app, game_t *game, client_helper_t *client)
{
    SDL_Event e;
    int result = STATE_CLIENT_SOCKET;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            if (client->is_connected)
                write_server(client->socketfd, &(client->sin), "exit");
            result = STATE_EXIT;
        }
        else if (e.type == SDL_KEYDOWN)
            result = client_event_keyboard(app, game, &e, client);
    }
    return (result);
}

static void client_waiting_draw(app_t *app, button_t *button)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    button_draw(app, button);
    SDL_RenderPresent(app->renderer);
}

static int init_connection(const char *address, int port, sockaddr_in_t *sin)
{
    hostent_t *hostinfo;
    int socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socketfd == INVALID_SOCKET) {
        perror("socket()");
        exit(errno);
    }
    printf("\033[0;32m=> Socket client has been created.\033[0m\n");
    printf("\033[0;33m=> Attempting to connect to %s...\033[0m\n", address);
    if ((hostinfo = gethostbyname(address)) == NULL) {
        fprintf(stderr, "\033[0;31mUnknown host %s.\033[0m\n", address);
        exit(EXIT_FAILURE);
    }
    sin->sin_addr = *(in_addr_bis_t *)hostinfo->h_addr;
    sin->sin_port = htons(port);
    sin->sin_family = AF_INET;
    return (socketfd);
}

int read_server(int socketfd, sockaddr_in_t *sin, char *buffer)
{
    int n = 0;
    socklen_t sinsize = sizeof(*sin);

    if ((n = recvfrom(socketfd, buffer, BUF_SIZE - 1, 0, (sockaddr_t *)sin, &sinsize)) < 0) {
        perror("recvfrom()");
        exit(errno);
    }
    buffer[n] = '\0';
    return (n);
}

void write_server(int socketfd, sockaddr_in_t *sin, const char *buffer)
{
    if (sendto(socketfd, buffer, strlen(buffer), 0, (sockaddr_t *)sin, sizeof(*sin)) < 0) {
        perror("sendto()");
        exit(errno);
    }
}

void client_interpet_message(app_t *app, game_t *game, char buffer[BUF_SIZE])
{
    int index = buffer[0] - '0';

    if (!strcmp(buffer + 2, "join"))
        game->players[index - 1]->is_alive = 1;
    else if (!strcmp(buffer + 2, "up"))
        move(app, game, index - 1, SDLK_UP);
    else if (!strcmp(buffer + 2, "down"))
        move(app, game, index - 1, SDLK_DOWN);
    else if (!strcmp(buffer + 2, "left"))
        move(app, game, index - 1, SDLK_LEFT);
    else if (!strcmp(buffer + 2, "right"))
        move(app, game, index - 1, SDLK_RIGHT);
    else if (!strcmp(buffer + 2, "bomb"))
        action(game, index - 1);
    else if (!strcmp(buffer + 2, "bomb")) {
        action(game, index - 1);
        game->players[index - 1]->is_alive = 0;
    }
}

static client_helper_t *client_create(app_t *app)
{
    client_helper_t *client = malloc(sizeof(client_helper_t));
    SDL_Rect button_pos1 = { 7.5 * app->tile_size, 10 * app->tile_size
                            , 10 * app->tile_size, 1.1 * app->tile_size };

    if (client == NULL) {
        fprintf(stderr, "Failed to malloc server\n");
        return (NULL);
    }
    client->socketfd = init_connection(app->ip, app->port, &(client->sin));
    client->is_connected = 0;
    client->max_index = client->socketfd;
    client->wait_text = button_create(app, "Connexion en cours...", button_pos1);
    return (client);
}

static void client_destroy(client_helper_t *client)
{
    if (client != NULL) {
        if (client->wait_text != NULL)
            button_destroy(client->wait_text);
        closesocket(client->socketfd);
        free(client);
    }
}

int client_run(app_t *app)
{
    client_helper_t *client = client_create(app);
    int state = STATE_CLIENT_SOCKET;
    char buffer[BUF_SIZE];
    fd_set rdfs;
    game_t *game = game_create(app);
    struct timeval waitd = { 0, 0 };

    memset(&(client->sin), 0, sizeof(client->sin));
    client->socketfd = init_connection(app->ip, app->port, &(client->sin));
    write_server(client->socketfd, &(client->sin), " ");
    while (state == STATE_CLIENT_SOCKET) {
        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(client->socketfd, &rdfs);
        if (select(client->socketfd + 1, &rdfs, NULL, NULL, &waitd) == -1) {
            perror("select()");
            exit(errno);
        }
        else if (FD_ISSET(client->socketfd, &rdfs)) {
            if ((client->max_index = read_server(client->socketfd, &(client->sin), buffer)) == 0) {
                printf("\033[0;31mServer disconnected !\033[0m\n");
                break;
            }
            if (!client->is_connected) {
                client->id = buffer[0] - '0';
                for (int i = 0; i < client->id; i++)
                    game->players[i]->is_alive = 1;
                client->is_connected = 1;
            }
            puts(buffer);
            client_interpet_message(app, game, buffer);
        }
        state = client_event(app, game, client);
        if (client->is_connected)
            game_draw(app, game);
        else
            client_waiting_draw(app, client->wait_text);
        SDL_Delay(20);
    }
    game_destroy(game);
    client_destroy(client);
    return (state);
}
