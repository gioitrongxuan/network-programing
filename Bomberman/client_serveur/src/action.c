/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_serveur
** File description:
**      Action function file
*/

#include "../inc/action.h"

int is_bomb_active_here(game_t *game, SDL_Point *coord)
{
    bomb_t *bomb = NULL;

    for (int i = 0; i < NB_PLAYERS; i++) {
        bomb = game->players[i]->bag->first;
        while (bomb != NULL) {
            if (bomb->is_active
                    && bomb->coord.x == coord->x && bomb->coord.y == coord->y)
                return (1);
            bomb = bomb->next;
        }
    }
    return (0);
}

int is_block_here(game_t *game, SDL_Point *coord)
{
    for (int i = 0; i < game->map->nb_block; i++)
        if (!game->map->block[i]->is_destroyed
            && game->map->block[i]->coord.x == coord->x
            && game->map->block[i]->coord.y == coord->y)
            return (1);
    return (0);
}

static void action_put_bomb(game_t *game, bomb_t *bomb, int index_player)
{
    if (!is_bomb_active_here(game, &(game->players[index_player]->coord))) {
        bomb->coord = game->players[index_player]->coord;
        bomb->position_rect = game->players[index_player]->position_rect;
        bomb->is_active = 1;
    }
}

void action(game_t *game, int index_player)
{
    bomb_t *bomb = game->players[index_player]->bag->first;

    while (bomb != NULL) {
        if (!bomb->is_active) {
            action_put_bomb(game, bomb, index_player);
            break;
        }
        bomb = bomb->next;
    }
}
