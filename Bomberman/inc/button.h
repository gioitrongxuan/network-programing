/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Button defintion file
*/

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef struct button_s {
    int selected;
    SDL_Rect position;
    SDL_Texture *texture_normal;
    SDL_Texture *texture_select;
} button_t;

void button_draw(button_t *, SDL_Renderer *);
void button_destroy(button_t *);
button_t *button_create(const char *, SDL_Rect
                        , SDL_Renderer *, TTF_Font *);
SDL_Texture *load_text_texture(const char *, SDL_Color *
                                , SDL_Renderer *, TTF_Font *);

#endif /* _BUTTON_H_ */
