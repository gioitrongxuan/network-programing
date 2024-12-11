/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Block function file
*/

#include "../inc/block.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Texture *load_block_texture(SDL_Renderer *renderer, const char *image)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = NULL;

    if (image != NULL)
        surface = IMG_Load(image);
    if (surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
        return (NULL);
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        return (NULL);
    }
    return (texture);
}

block_t *block_load(SDL_Texture *texture, block_type_t block_type
                    ,int x, int y, int tile_size)
{
    block_t *block = malloc(sizeof(block_t));

    if (block == NULL)
        return (NULL);
    block->is_destroyed = 0;
    block->block_type = block_type;
    block->coord.x = x;
    block->coord.y = y;
    block->position_rect.x = block->coord.x * tile_size;
    block->position_rect.y = block->coord.y * tile_size;
    block->position_rect.w = tile_size;
    block->position_rect.h = tile_size;
    block->texture = texture;
    return (block);
}

void block_draw(block_t *block, SDL_Renderer *renderer)
{
    if (!block->is_destroyed)
        SDL_RenderCopy(renderer, block->texture, NULL, &(block->position_rect));
}

void block_destroy(block_t *block)
{
    if (block != NULL) {
        free(block);
    }
}
