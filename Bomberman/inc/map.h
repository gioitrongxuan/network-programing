/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Map defintion file
*/

#ifndef _MAP_H_
#define _MAP_H_

#include "block.h"

typedef struct map_s {
    int i_max;
    int j_max;
    int nb_block;
    SDL_Texture *wall_texture;
    SDL_Texture *block_texture;
    SDL_Texture *floor_texture;
    block_t **block;
} map_t;


map_t *map_load(SDL_Renderer *, int, const char *);
void map_draw(map_t *, SDL_Renderer *, int);
void map_destroy(map_t *);
#endif /* _MAP_H_ */
