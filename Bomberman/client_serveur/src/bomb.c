/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_serveur
** File description:
**      Bomb function file
*/

#include "../inc/bag.h"

#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

static bomb_t *bomb_init(int tile_size)
{
    bomb_t *bomb = malloc(sizeof(bomb_t));

    if (bomb == NULL)
        return (NULL);
    bomb->is_active = 0;
    bomb->coord.x = 0;
    bomb->coord.y = 0;
    bomb->position_rect.x = bomb->coord.x * tile_size;
    bomb->position_rect.y = bomb->coord.y * tile_size;
    bomb->position_rect.w = tile_size;
    bomb->position_rect.h = tile_size;
    bomb->texture = NULL;
    return (bomb);
}

bomb_t *bomb_load(SDL_Renderer *renderer
                    , int tile_size, const char *file)
{
    bomb_t *bomb = bomb_init(tile_size);
    SDL_Surface *surface = NULL;

    if (bomb == NULL) {
        fprintf(stderr, "Failed to create bomb\n");
        return (NULL);
    }
    if (file != NULL)
        surface = IMG_Load(file);
    if (surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
        free(bomb);
        return (NULL);
    }
    bomb->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (bomb->texture == NULL) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        free(bomb);
        return (NULL);
    }
    return (bomb);
}

void bomb_draw(bomb_t *bomb, SDL_Renderer *renderer)
{
    if (bomb->is_active)
        SDL_RenderCopy(renderer, bomb->texture, NULL, &(bomb->position_rect));
}

void bomb_destroy(bomb_t *bomb)
{
    if (bomb != NULL) {
        if (bomb->texture != NULL)
            SDL_DestroyTexture(bomb->texture);
        free(bomb);
    }
}
