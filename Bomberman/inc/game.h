/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Game definition file
*/

#ifndef _GAME_H_
#define _GAME_H_

#include <SDL2/SDL.h>

#include "app.h"
#include "button.h"
#include "explosion_queue.h"
#include "map.h"
#include "player.h"

#define NB_PLAYERS 4

typedef struct game_s {
    // Objects
    button_t *chrono;
    map_t *map;
    player_t *players[4];
    explosion_list_t *explo_queue;

    // Settings
    int time_enabled;
    int status;
    int is_bonus;
    int max_bombs;
    long chrono_time;
    Uint32 current_time;
    Uint32 time_left;
    Uint32 time_started;
    const char *map_selected;
} game_t;

game_t *game_create(app_t *);
void game_draw(app_t *, game_t *);
int game_update(app_t *, game_t *);
void game_update_chrono(app_t *, game_t *);
int game_is_over(game_t *game);
void game_destroy(game_t *);

#endif /* _GAME_H_ */
