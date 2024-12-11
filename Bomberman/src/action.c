/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Action function file
*/

#include "../inc/action.h"

bomb_t *get_bomb_active_here(game_t *game, SDL_Point *coord)
{
    bomb_t *bomb = NULL;

    for (int i = 0; i < NB_PLAYERS; i++) {
        bomb = game->players[i]->bag->first;
        while (bomb != NULL) {
            if (bomb->is_active
                    && bomb->coord.x == coord->x && bomb->coord.y == coord->y)
                return (bomb);
            bomb = bomb->next;
        }
    }
    return (NULL);
}

block_t *get_block_undestroyed_here(game_t *game, SDL_Point *coord)
{
    for (int i = 0; i < game->map->nb_block; i++)
        if (!game->map->block[i]->is_destroyed
            && game->map->block[i]->coord.x == coord->x
            && game->map->block[i]->coord.y == coord->y)
            return (game->map->block[i]);
    return (NULL);
}

player_t *get_player_alive_here(game_t *game, SDL_Point *coord)
{
    for (int i = 0; i < NB_PLAYERS; i++)
        if (game->players[i]->is_alive
            && game->players[i]->coord.x == coord->x
            && game->players[i]->coord.y == coord->y)
            return (game->players[i]);
    return (NULL);
}

static void action_put_bomb(game_t *game, bomb_t *bomb, int index_player)
{
    if (get_bomb_active_here(game, &(game->players[index_player]->coord)) == NULL) {
        bomb->coord = game->players[index_player]->coord;
        bomb->position_rect = game->players[index_player]->position_rect;
        bomb->is_active = 1;
        bomb->time_activated = SDL_GetTicks();
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
