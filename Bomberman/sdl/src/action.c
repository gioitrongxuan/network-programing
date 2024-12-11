/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Action function file
*/

#include "../inc/action.h"

int is_bomb_active_here(game_t *game, SDL_Point *coord)
{
    bomb_t *bomb = game->player1->bag->first;

    while (bomb != NULL) {
        if (bomb->is_active
                && bomb->coord.x == coord->x && bomb->coord.y == coord->y)
            return (1);
        bomb = bomb->next;
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

static void action_put_bomb(game_t *game, bomb_t *bomb)
{
    if (!is_bomb_active_here(game, &(game->player1->coord))) {
        bomb->coord = game->player1->coord;
        bomb->position_rect = game->player1->position_rect;
        bomb->is_active = 1;
    }
}

void action(game_t *game)
{
    bomb_t *bomb = game->player1->bag->first;

    while (bomb != NULL) {
        if (!bomb->is_active) {
            action_put_bomb(game, bomb);
            break;
        }
        bomb = bomb->next;
    }
}
