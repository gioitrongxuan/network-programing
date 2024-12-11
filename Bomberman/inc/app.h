/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      App defintion file
*/

#ifndef _APP_H_
#define _APP_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define NB_MAP 2

typedef struct app_s {
    Uint32 time_left;
    int nb_bomb_start;
    int port;
    int index_map;
    const char *ip;
    const char *maps_available[NB_MAP];
    int tile_size;
    TTF_Font *font;
    SDL_Point screen_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
} app_t;

app_t *app_start(int, int, int);
void app_destroy(app_t *);

#endif /* _APP_H_ */
