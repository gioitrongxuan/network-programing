/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_serveur
** File description:
**      Game definition file
*/

#ifndef _GAME_H_
#define _GAME_H_

#include <SDL2/SDL.h>

#include "app.h"
#include "map.h"
#include "player.h"

#define NB_PLAYERS 4

typedef struct game_s {
    // Objects
    map_t *map;
    player_t *players[4];

    // Settings
    int max_bombs;
} game_t;

game_t *game_create(app_t *);
void game_draw(app_t *, game_t *);
void game_destroy(game_t *);

#endif /* _GAME_H_ */
