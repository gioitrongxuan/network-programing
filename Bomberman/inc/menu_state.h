/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Menu defintion file
*/

#ifndef _MENU_STATE_H_
#define _MENU_STATE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "button.h"

typedef struct menu_s {
    int index_select;
    SDL_Texture *texture;
    button_t *buttons[3];
} menu_t;

int menu_run(app_t *);

#endif /* _MENU_STATE_H_ */
