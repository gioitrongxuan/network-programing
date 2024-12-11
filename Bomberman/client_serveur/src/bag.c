/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_serveur
** File description:
**      Bag function file
*/

#include "../inc/bag.h"

#include <stdlib.h>

static bag_t *bag_init()
{
    bag_t *bag = malloc(sizeof(bag_t));

    if (bag == NULL)
        return (NULL);
    bag->first = NULL;
    bag->last = NULL;
    bag->size = 0;
    return (bag);
}

static int bag_add(bag_t *bag, SDL_Renderer *renderer, int tilesize
                    , const char *file)
{
    bomb_t *new_node = bomb_load(renderer, tilesize, file);

    if (new_node == NULL)
        return (0);
    new_node->prev = NULL;
    new_node->next = bag->first;
    if (bag->first)
        bag->first->prev = new_node;
    else
        bag->last = new_node;
    bag->first = new_node;
    bag->size++;
    return (1);
}

bag_t *bag_load(SDL_Renderer *renderer, int tile_size, int nb_bomb,
    const char *file)
{
    bag_t *bag = bag_init();
    for (int i = 0; i < nb_bomb; i++) {
        if (!bag_add(bag, renderer, tile_size, file))
            return (NULL);
    }
    return (bag);
}


void bag_draw(bag_t *bag, SDL_Renderer *renderer)
{
    bomb_t *bomb = bag->first;

    while (bomb != NULL) {
        bomb_draw(bomb, renderer);
        bomb = bomb->next;
    }
}


void bag_destroy(bag_t *bag)
{
    bomb_t *next;
    bomb_t *current = bag->first;

    while (current != NULL) {
        next = current->next;
        bomb_destroy(current);
        current = next;
    }
    free(bag);
}
