/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      App defintion file
*/

#ifndef _APP_H_
#define _APP_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct app_s {
    int port;
    const char *ip;
    int tile_size;
    TTF_Font *font;
    SDL_Point screen_size;
    SDL_Window *window;
    SDL_Renderer *renderer;
} app_t;

app_t *app_start(int, int, int);
void app_destroy(app_t *);

#endif /* _APP_H_ */
