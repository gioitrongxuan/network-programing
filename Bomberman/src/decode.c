/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Decode function file
*/

#include "../inc/client.h"

#include <stdio.h>
#include <string.h>

static void decode_map(game_t *game, char *payload)
{
    for (int i = 0; payload[i] != '\0'; i++)
        game->map->block[i]->is_destroyed = payload[i] - '0';
}

static void decode_bomb(app_t *app, bomb_t *bomb, char *payload, int *j)
{
    char tmp_x[4] = { 0 };
    char tmp_y[4] = { 0 };

    bomb->is_active = payload[*j] - '0';
    *j += 2;
    for (int k = 0; payload[*j] != ','; k++) {
        tmp_x[k] = payload[*j];
        *j += 1;
    }
    *j += 1;
    bomb->coord.x = atoi(tmp_x);
    for (int k = 0; payload[*j] != ',' && payload[*j] != 0; k++) {
        tmp_y[k] = payload[*j];
        *j += 1;
    }
    *j += 1;
    bomb->coord.y = atoi(tmp_y);
    bomb_update_pos(bomb, app->tile_size);
}

static void decode_bag(app_t *app, game_t *game, int indexplayer, char *payload
                        , int *j)
{
    int nb_bombs = 0;
    char nb_bombs_str[4] = { 0 };
    bomb_t *bomb = NULL;

    for (int k = 0; payload[*j] != ','; k++) {
        nb_bombs_str[k] = payload[*j];
        *j += 1;
    }
    *j += 1;
    nb_bombs = atoi(nb_bombs_str);
    for (int i = game->players[indexplayer]->bag->size; i < nb_bombs; i++) {
        if (!bag_add(game->players[indexplayer]->bag, app->renderer
            , app->tile_size, "./rsc/sprites/bomb/bomb.png"))
            return;
    }
    bomb = game->players[indexplayer]->bag->first;
    for (; bomb != NULL; ) {
        decode_bomb(app, bomb, payload, j);
        bomb = bomb->next;
    }
}

static void decode_player(app_t *app, game_t *game, int index_player
                            , char *payload)
{
    int j = 2;
    char tmp_x[4] = { 0 };
    char tmp_y[4] = { 0 };

    game->players[index_player]->is_alive = payload[0] - '0';
    for (int k = 0; payload[j] != ','; k++) {
        tmp_x[k] = payload[j];
        j++;
    }
    j++;
    game->players[index_player]->coord.x = atoi(tmp_x);
    for (int k = 0; payload[j] != ','; k++) {
        tmp_y[k] = payload[j];
        j++;
    }
    j++;
    game->players[index_player]->coord.y = atoi(tmp_y);
    player_update_pos(game->players[index_player], app->tile_size);
    decode_bag(app, game, index_player, payload, &j);
}

static void decode_explosion_create(app_t *app, game_t *game, SDL_Point *coord
                                    , explo_type_t explo_type)
{
    switch (explo_type) {
    default:
    case EXPLO_CENTER:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/center.png", app->renderer);
        break;
    case EXPLO_TOP:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/top.png", app->renderer);
        break;
    case EXPLO_DOWN:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/down.png", app->renderer);
        break;
    case EXPLO_LEFT:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/left.png", app->renderer);
        break;
    case EXPLO_RIGHT:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/right.png", app->renderer);
        break;
    case EXPLO_VERT:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/vert.png", app->renderer);
        break;
    case EXPLO_HORI:
        add_explosion(game->explo_queue, coord, app->tile_size, explo_type
                            , "./rsc/sprites/bomb/explosion/hori.png", app->renderer);
        break;
    }
}

static void decode_explosion(app_t *app, game_t *game, int *j
                            , char *payload)
{
    SDL_Point coord = { 0, 0 };
    explo_type_t explo_type = EXPLO_CENTER;
    char tmp_x[4] = { 0 };
    char tmp_y[4] = { 0 };

    explo_type = payload[*j] - '0';
    *j += 2;
    for (int k = 0; payload[*j] != ','; k++) {
        tmp_x[k] = payload[*j];
        *j += 1;
    }
    *j += 1;
    coord.x = atoi(tmp_x);
    for (int k = 0; payload[*j] != ',' && payload[*j] != 0; k++) {
        tmp_y[k] = payload[*j];
        *j += 1;
    }
    *j += 1;
    coord.y = atoi(tmp_y);
    decode_explosion_create(app, game, &coord, explo_type);
}

static void decode_explosions(app_t *app, game_t *game, char *payload)
{
    int j = 0;
    int nb_explo = 0;
    char nb_explo_str[4] = { 0 };

    for (int k = 0; payload[j] != ',' && payload[j] != 0; k++) {
        nb_explo_str[k] = payload[j];
        j++;
    }
    j++;
    nb_explo = atoi(nb_explo_str);
    for (; game->explo_queue->size; )
        stop_explosion(game->explo_queue);
    for (int i = 0; i < nb_explo; i++)
        decode_explosion(app, game, &j, payload);
}

void decode_game(app_t *app, game_t *game, char *payload)
{
    int j = 0;
    char payload_explosions[1024] = { 0 };

    for (int i = 0; i < 4; i++) {
        char tmp_payload[512] = { 0 };

        for (int k = 0; payload[j] != '\n'; k++) {
            tmp_payload[k] = payload[j];
            j++;
        }
        decode_player(app, game, i, tmp_payload);
        j++;
    }
    for (int k = 0; payload[j] != '\n'; k++) {
        payload_explosions[k] = payload[j];
        j++;
    }
    decode_explosions(app, game, payload_explosions);
    game->status = payload[++j] - '0';
    j += 2;
    decode_map(game, payload + j);
}

void decode_notif(app_t *app, char *payload)
{
    int j = 0;
    char tmp_bombs[4] = { 0 };
    char time[4] = { 0 };

    for (int k = 0; payload[j] != ' '; k++) {
        tmp_bombs[k] = payload[j];
        j++;
    }
    app->nb_bomb_start = atoi(tmp_bombs);
    j++;
    for (int k = 0; payload[j] != ' '; k++) {
        time[k] = payload[j];
        j++;
    }
    app->time_left = atoi(time);
    j++;
    app->index_map = atoi(payload + j);
    j++;
}
