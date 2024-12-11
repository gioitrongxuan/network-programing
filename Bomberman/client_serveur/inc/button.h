/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Button defintion file
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "app.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct button_s {
    int selected;
    SDL_Rect position;
    SDL_Texture *texture_normal;
    SDL_Texture *texture_select;
} button_t;

SDL_Texture *load_text_texture(app_t *, const char *, SDL_Color *);

button_t *button_create(app_t *, const char *, SDL_Rect);
void button_draw(app_t *, button_t *);
void button_destroy(button_t *);

#endif /* _BUTTON_H_ */
