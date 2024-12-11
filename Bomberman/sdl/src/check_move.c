/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Check if player can move function file
*/

#include "../inc/action.h"
#include "../inc/check_move.h"

int check_move_up(game_t *game)
{
    SDL_Point to_go = { game->player1->coord.x, game->player1->coord.y - 1 };

    if (game->player1->coord.y * game->tile_size - 1 * game->tile_size < 0)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_down(game_t *game)
{
    SDL_Point to_go = { game->player1->coord.x, game->player1->coord.y + 1 };

    if (game->player1->coord.y * game->tile_size + 1 * game->tile_size
                            > game->screen_size.y - 1 * game->tile_size)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_left(game_t *game)
{
    SDL_Point to_go = { game->player1->coord.x - 1, game->player1->coord.y };

    if (game->player1->coord.x * game->tile_size - 1 * game->tile_size < 0)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_right(game_t *game)
{
    SDL_Point to_go = { game->player1->coord.x + 1, game->player1->coord.y };

    if (game->player1->coord.x * game->tile_size + 1 * game->tile_size
                            > game->screen_size.x - 1 * game->tile_size)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}
