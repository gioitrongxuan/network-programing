/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Explosion queue defintion file
*/

#ifndef _EXPLOSION_QUEUE_H_
#define _EXPLOSION_QUEUE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef enum explo_type_e {
    EXPLO_CENTER,
    EXPLO_TOP,
    EXPLO_DOWN,
    EXPLO_LEFT,
    EXPLO_RIGHT,
    EXPLO_VERT,
    EXPLO_HORI
} explo_type_t;

typedef struct explosion_s {
    explo_type_t explo_type;
    Uint32 time_explosed;
    SDL_Point coord;
    SDL_Rect position_rect;
    SDL_Texture *texture;
    struct explosion_s *next;
} explosion_t;

typedef struct explosion_list_s {
    int size;
    explosion_t *front;
    explosion_t *rear;
} explosion_list_t;

explosion_list_t *create_list_explosion(void);
void add_explosion(explosion_list_t *, SDL_Point *, int, explo_type_t
                    , const char *, SDL_Renderer *);
void stop_explosion(explosion_list_t *);
void destroy_list_explosion(explosion_list_t *);

void explosions_draw(explosion_list_t *q, SDL_Renderer *renderer);

#endif /* _EXPLOSION_QUEUE_H_ */
