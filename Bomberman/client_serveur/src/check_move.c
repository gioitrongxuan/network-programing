/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Check if player can move function file
*/

#include "../inc/action.h"
#include "../inc/check_move.h"

int check_move_up(app_t *app, game_t *game, int index_player)
{
    SDL_Point to_go = { game->players[index_player]->coord.x, game->players[index_player]->coord.y - 1 };

    if (game->players[index_player]->coord.y * app->tile_size - 1 * app->tile_size < 0)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_down(app_t *app, game_t *game, int index_player)
{
    SDL_Point to_go = { game->players[index_player]->coord.x, game->players[index_player]->coord.y + 1 };

    if (game->players[index_player]->coord.y * app->tile_size + 1 * app->tile_size
                            > app->screen_size.y - 1 * app->tile_size)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_left(app_t *app, game_t *game, int index_player)
{
    SDL_Point to_go = { game->players[index_player]->coord.x - 1, game->players[index_player]->coord.y };

    if (game->players[index_player]->coord.x * app->tile_size - 1 * app->tile_size < 0)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}

int check_move_right(app_t *app, game_t *game, int index_player)
{
    SDL_Point to_go = { game->players[index_player]->coord.x + 1, game->players[index_player]->coord.y };

    if (game->players[index_player]->coord.x * app->tile_size + 1 * app->tile_size
                            > app->screen_size.x - 1 * app->tile_size)
        return (0);
    if (is_bomb_active_here(game, &to_go))
        return (0);
    if (is_block_here(game, &to_go))
        return (0);
    return (1);
}
