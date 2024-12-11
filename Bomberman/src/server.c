/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Server function file
*/

#include "../inc/action.h"
#include "../inc/move.h"
#include "../inc/server.h"
#include "../inc/state.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int server_event_keyboard(app_t *app, game_t *game, SDL_Event *e
        , server_t *server)
{
    int result = STATE_SERVER_SOCKET;

    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
        result = STATE_SERVER;
        break;
    case SDLK_UP:
        if (server->game_started) {
            move(app, game, 0, SDLK_UP);
            send_all_clients(server, game);
        }
        break;
    case SDLK_DOWN:
        if (server->game_started) {
            move(app, game, 0, SDLK_DOWN);
            send_all_clients(server, game);
        }
        break;
    case SDLK_LEFT:
        if (server->game_started) {
            move(app, game, 0, SDLK_LEFT);
            send_all_clients(server, game);
        }
        break;
    case SDLK_RIGHT:
        if (server->game_started) {
            move(app, game, 0, SDLK_RIGHT);
            send_all_clients(server, game);
        }
        break;
    case SDLK_SPACE:
        if (server->game_started) {
            action(game, 0);
            send_all_clients(server, game);
        }
        break;
    default:
        break;
    }
    return (result);
}

static int server_event(app_t *app, game_t *game, server_t *server)
{
    SDL_Event e;
    int result = STATE_SERVER_SOCKET;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            result = STATE_EXIT;
        else if (e.type == SDL_KEYDOWN) {
            result = server_event_keyboard(app, game, &e, server);
        }
    }
    return (result);
}

static void server_draw(app_t *app, server_t *server)
{
    if (server->game_is_starting)
        button_draw(server->start_text, app->renderer);
    else if (!server->game_started)
        button_draw(server->wait_text, app->renderer);
    SDL_RenderPresent(app->renderer);
}

static void server_interpet_message(app_t *app, game_t *game, char buffer[BUF_SIZE]
                                , int index, server_t *server)
{
    if (server->game_started) {
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
    }
    if (!strcmp(buffer, "exit")) {
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
    server->socketfd = server_connection(app->port, &(server->sin));
    server->actual_index = 0;
    server->game_started = 0;
    server->game_is_starting = 0;
    server->max_index = server->socketfd;
    server->wait_text = button_create("En attente de joueurs...", button_pos1, app->renderer, app->font);
    server->wait_text->selected = 1;
    server->start_text = button_create("La partie va demarrer...", button_pos1, app->renderer, app->font);
    server->start_text->selected = 1;
    create_client(server, &(server->sin));
    return (server);
}

static void server_destroy(server_t *server)
{
    if (server != NULL) {
        if (server->wait_text != NULL)
            button_destroy(server->wait_text);
        if (server->start_text != NULL)
            button_destroy(server->start_text);
        closesocket(server->socketfd);
        free(server);
    }
}

static int server_time(server_t *server, game_t *game)
{
    int over = 0;

    if (server->game_is_starting) {
        if (!game_is_over(game)) {
            if (SDL_GetTicks() >= server->at_game_starting + 15 * 1000) {
                server->game_started = 1;
                server->game_is_starting = 0;
                game->status = 1;
                game->time_started = SDL_GetTicks();
                game->time_enabled = (game->time_left) ? 1 : 0;
                send_all_clients(server, game);
            }
        } else {
            server->game_is_starting = 0;
        }
    }
    else if (server->game_started) {
        if (game_is_over(game)) {
            over = 1;
            game->status = 2;
            send_all_clients(server, game);
        }
    }
    return (over);
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
                    printf("player%d: '%s'\n", client->index, buffer);
                    server_interpet_message(app, game, buffer, client->index
                                            , server);
                    send_all_clients(server, game);
                }
            } else if (server->actual_index != MAX_CLIENTS) {
                if (!server->game_started) {
                    client_t tmp_client = create_client(server, &csin);
                    game->players[tmp_client.index - 1]->is_alive = 1;
                    send_notif_join(server, &tmp_client, app);
                    send_all_clients(server, game);
                    printf("Player %d joined the game\n", tmp_client.index);
                    if (!server->game_is_starting) {
                        server->at_game_starting = SDL_GetTicks();
                        server->game_is_starting = 1;
                    }
                }
            }
        }
        if (game_update(app, game))
            send_all_clients(server, game);
        if (server_time(server, game))
            state = STATE_MENU;
        else
            state = server_event(app, game, server);
        game_draw(app, game);
        server_draw(app, server);
        SDL_Delay(20);
    }
    game_destroy(game);
    server_destroy(server);
    return (state);
}
