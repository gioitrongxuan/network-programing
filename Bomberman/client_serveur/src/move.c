/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Move function file
*/

#include "../inc/check_move.h"
#include "../inc/move.h"

static void move_up(app_t *app, game_t *game, int index_player)
{
    game->players[index_player]->coord.y -= 1;
    game->players[index_player]->position_rect.y -= (1 * app->tile_size);
}

static void move_down(app_t *app, game_t *game, int index_player)
{
    game->players[index_player]->coord.y += 1;
    game->players[index_player]->position_rect.y += (1 * app->tile_size);
}

static void move_left(app_t *app, game_t *game, int index_player)
{
    game->players[index_player]->coord.x -= 1;
    game->players[index_player]->position_rect.x -= (1 * app->tile_size);
}

static void move_right(app_t *app, game_t *game, int index_player)
{
    game->players[index_player]->coord.x += 1;
    game->players[index_player]->position_rect.x += (1 * app->tile_size);
}

void move(app_t *app, game_t *game, int index_player, SDL_Keycode direction)
{
    switch (direction)
    {
    case SDLK_UP:
        if (check_move_up(app, game, index_player))
            move_up(app, game, index_player);
        break;
    case SDLK_DOWN:
        if (check_move_down(app, game, index_player))
            move_down(app, game, index_player);
        break;
    case SDLK_LEFT:
        if (check_move_left(app, game, index_player))
            move_left(app, game, index_player);
        break;
    case SDLK_RIGHT:
        if (check_move_right(app, game, index_player))
            move_right(app, game, index_player);
        break;
    default:
        break;
    }
}
