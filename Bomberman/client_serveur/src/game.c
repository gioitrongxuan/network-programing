/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_serveur
** File description:
**      Game function file
*/

#include "../inc/game.h"

#include <stdio.h>
#include <stdlib.h>

static int game_create_players(app_t *app, game_t *game)
{
    game->players[0] = player_load(app->renderer, 1, 1, app->tile_size,
                                                        "./rsc/player.png");
    game->players[1] = player_load(app->renderer, 23, 1, app->tile_size,
                                                        "./rsc/player.png");
    game->players[2] = player_load(app->renderer, 23, 19, app->tile_size,
                                                        "./rsc/player.png");
    game->players[3] = player_load(app->renderer, 1, 19, app->tile_size,
                                                        "./rsc/player.png");
    if (game->players[0] == NULL || game->players[1] == NULL
        || game->players[2] == NULL || game->players[3] == NULL)
        return (0);
    return (1);
}

game_t *game_create(app_t *app)
{
    game_t *game = malloc(sizeof(game_t));

    if (game == NULL) {
        fprintf(stderr, "Failed to malloc game");
        return (NULL);
    }
    game->max_bombs = 2;
    game->map = map_load(app->renderer, app->tile_size, "./rsc/map.txt");
    if (!game_create_players(app, game) || game->map == NULL)
        return (NULL);
    return (game);
}

void game_destroy(game_t *game)
{
    if (game != NULL) {
        for (int i = 0; i < NB_PLAYERS; i++)
            if (game->players[i] != NULL)
                player_destroy(game->players[i]);
        if (game->map != NULL)
            map_destroy(game->map);
        free(game);
    }
}

void game_draw(app_t *app, game_t *game)
{
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    map_draw(game->map, app->renderer, app->tile_size);
    for (int i = 0; i < NB_PLAYERS; i++) {
        player_draw(game->players[i], app->renderer);
    }
    SDL_RenderPresent(app->renderer);
}
