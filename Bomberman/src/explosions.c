/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\temperman
** File description:
**      Explosions function file
*/

#include "../inc/explosion_queue.h"

#include <stdio.h>
#include <stdlib.h>

explosion_list_t *create_list_explosion(void)
{
    explosion_list_t *q = malloc(sizeof(explosion_list_t));

    q->size = 0;
    q->front = NULL;
    q->rear = NULL;
    return (q);
}

static explosion_t *new_node(const char *file, SDL_Renderer *renderer)
{
    SDL_Surface *surface = NULL;
    explosion_t *temp = malloc(sizeof(explosion_t));

    if (temp == NULL) {
        fprintf(stderr, "Failed to create explosion\n");
        return (NULL);
    }
    if (file != NULL)
        surface = IMG_Load(file);
    if (surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
        free(temp);
        return (NULL);
    }
    temp->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (temp->texture == NULL) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        free(temp);
        return (NULL);
    }
    return (temp);
}

void add_explosion(explosion_list_t *q, SDL_Point *coord, int tilesize
                    , explo_type_t explo_type, const char *f
                    , SDL_Renderer *renderer)
{
    explosion_t *temp = new_node(f, renderer);

    if (temp == NULL)
        return;
    temp->next = NULL;
    temp->coord.x = coord->x;
    temp->coord.y = coord->y;
    temp->explo_type = explo_type;
    temp->position_rect.x = temp->coord.x * tilesize;
    temp->position_rect.y = temp->coord.y * tilesize;
    temp->position_rect.w = tilesize;
    temp->position_rect.h = tilesize;
    temp->time_explosed = SDL_GetTicks();
    q->size++;
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

void stop_explosion(explosion_list_t *q)
{
    explosion_t *temp = NULL;

    if (q->front == NULL)
        return;
    temp = q->front;
    q->front = q->front->next;
    if (q->front == NULL)
        q->rear = NULL;
    q->size--;
    if (temp->texture != NULL)
        SDL_DestroyTexture(temp->texture);
    free(temp);
}

void explosions_draw(explosion_list_t *q, SDL_Renderer *renderer)
{
    explosion_t *temp = q->front;

    for (int i = 0; i < q->size && temp != NULL; i++) {
        SDL_RenderCopy(renderer, temp->texture, NULL, &(temp->position_rect));
        temp = temp->next;
    }
}

void destroy_list_explosion(explosion_list_t *q)
{
    explosion_t *next = NULL;
    explosion_t *current = q->front;

    while (current != NULL) {
        next = current->next;
        if (current->texture != NULL)
            SDL_DestroyTexture(current->texture);
        free(current);
        current = next;
    }
    free(q);
}
