/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Button function file
*/

#include "../inc/button.h"

#include <stdio.h>
#include <stdlib.h>

SDL_Texture *load_text_texture(const char *text, SDL_Color *color
                                , SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, *color);

    if (surface == NULL) {
        fprintf(stderr, "Failed to load image text: %s\n", TTF_GetError());
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

button_t *button_create(const char *text, SDL_Rect rect
                        , SDL_Renderer *renderer, TTF_Font *font)
{
    button_t *button = malloc(sizeof(button_t));
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Color color_selected = { 255, 0, 0, 255 };

    if (button == NULL) {
        fprintf(stderr, "Failed to malloc button\n");
        return (NULL);
    }
    button->selected = 0;
    button->position = rect;
    button->texture_normal = load_text_texture(text, &color, renderer, font);
    button->texture_select = load_text_texture(text, &color_selected, renderer, font);
    return (button);
}

void button_draw(button_t *button, SDL_Renderer *renderer)
{
    if (button->selected)
        SDL_RenderCopy(renderer, button->texture_select, NULL, &button->position);
    else
        SDL_RenderCopy(renderer, button->texture_normal, NULL, &button->position);
}

void button_destroy(button_t *button)
{
    if (button != NULL) {
        if (button->texture_normal != NULL)
            SDL_DestroyTexture(button->texture_normal);
        if (button->texture_select != NULL)
            SDL_DestroyTexture(button->texture_select);
        free(button);
    }
}
