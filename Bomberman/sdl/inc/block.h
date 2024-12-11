/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Block defintion file
*/

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <SDL2/SDL.h>

typedef enum block_type {
    BLOCK_METAL,
    BLOCK_NORMAL
} block_type_t;

typedef struct block_s {
    int is_destroyed;
    block_type_t block_type;
    SDL_Point coord;
    SDL_Rect position_rect;
    SDL_Texture *texture;
} block_t;

block_t *block_load(SDL_Texture *, int, int, int);
void block_draw(block_t *, SDL_Renderer *);
void block_destroy(block_t *);

SDL_Texture *load_block_texture(SDL_Renderer *, const char *);

#endif /* _BLOCK_H_ */
