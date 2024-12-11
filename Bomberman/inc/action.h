/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Action defintion file
*/

#ifndef _ACTION_H_
#define _ACTION_H_

#include "game.h"

void action(game_t *, int);
player_t *get_player_alive_here(game_t *, SDL_Point *);
bomb_t *get_bomb_active_here(game_t *, SDL_Point *);
block_t *get_block_undestroyed_here(game_t *, SDL_Point *);

#endif /* _ACTION_H_ */
