/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Client state defintion file
*/

#ifndef _client_STATE_H_
#define _client_STATE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "button.h"

typedef struct client_state_s {
    int index_select;
    SDL_Texture *texture;
    button_t *buttons[6];
} client_state_t;

int client_state_run(app_t *);

#endif /* _client_STATE_H_ */
