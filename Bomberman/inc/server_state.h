/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Server state defintion file
*/

#ifndef _SERVER_STATE_H_
#define _SERVER_STATE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "button.h"

typedef struct server_state_s {
    int index_select;
    SDL_Texture *texture;
    button_t *buttons[10];
} server_state_t;

int server_state_run(app_t *);

#endif /* _SERVER_STATE_H_ */
