/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Move function file
*/

#include "../inc/check_move.h"
#include "../inc/move.h"

static void move_up(game_t *game)
{
    game->player1->coord.y -= 1;
    game->player1->position_rect.y -= (1 * game->tile_size);
}

static void move_down(game_t *game)
{
    game->player1->coord.y += 1;
    game->player1->position_rect.y += (1 * game->tile_size);
}

static void move_left(game_t *game)
{
    game->player1->coord.x -= 1;
    game->player1->position_rect.x -= (1 * game->tile_size);
}

static void move_right(game_t *game)
{
    game->player1->coord.x += 1;
    game->player1->position_rect.x += (1 * game->tile_size);
}

void move(game_t *game, SDL_Keycode direction)
{
    switch (direction)
    {
    case SDLK_UP:
        if (check_move_up(game))
            move_up(game);
        break;
    case SDLK_DOWN:
        if (check_move_down(game))
            move_down(game);
        break;
    case SDLK_LEFT:
        if (check_move_left(game))
            move_left(game);
        break;
    case SDLK_RIGHT:
        if (check_move_right(game))
            move_right(game);
        break;
    default:
        break;
    }
}
