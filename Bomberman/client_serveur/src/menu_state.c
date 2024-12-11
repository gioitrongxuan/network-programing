/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      Menu function file
*/

#include "../inc/app.h"
#include "../inc/menu_state.h"
#include "../inc/state.h"

#include <stdio.h>
#include <stdlib.h>

static SDL_Texture *menu_background(app_t *app)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = NULL;

    surface = IMG_Load("./rsc/menu.png");
    if (surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", SDL_GetError());
        return (NULL);
    }
    texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        return (NULL);
    }
    return (texture);

}

static menu_t *menu_create(app_t *app)
{
    menu_t *menu = malloc(sizeof(menu_t));
    SDL_Rect button_pos1 = { 1.5 * app->tile_size, 10 * app->tile_size, 8.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos2 = { 1.5 * app->tile_size, 12 * app->tile_size, 9.5 * app->tile_size, 1.1 *app->tile_size };
    SDL_Rect button_pos3 = { 1.5 * app->tile_size, 14 * app->tile_size, 3.1 * app->tile_size, 1.1 * app->tile_size };

    if (menu == NULL) {
        fprintf(stderr, "Failed to malloc menu\n");
        return (NULL);
    }
    menu->texture = menu_background(app);
    if (menu->texture == NULL)
        return(NULL);
    menu->buttons[0] = button_create(app, "Heberger une partie", button_pos1);
    menu->buttons[1] = button_create(app, "Se connecter a une partie", button_pos2);
    menu->buttons[2] = button_create(app, "Sortir", button_pos3);
    menu->index_select = 0;
    menu->buttons[menu->index_select]->selected = 1;
    return (menu);
}

static void menu_draw(app_t *app, menu_t *menu)
{
    SDL_Rect full_screen = { 0, 0, app->screen_size.x, app->screen_size.y };

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderCopy(app->renderer, menu->texture, NULL, &full_screen);
    for (int i = 0; i < 3; i++)
        button_draw(app, menu->buttons[i]);
    SDL_RenderPresent(app->renderer);
}

static void move_up(menu_t *menu)
{
    if (menu->index_select > 0) {
        menu->buttons[menu->index_select--]->selected = 0;
        menu->buttons[menu->index_select]->selected = 1;
    }
}

static void move_down(menu_t *menu)
{
    if (menu->index_select < 2) {
        menu->buttons[menu->index_select++]->selected = 0;
        menu->buttons[menu->index_select]->selected = 1;
    }
}

static int menu_enter(menu_t *menu)
{
    switch (menu->index_select) {
    case 0:
        return (STATE_SERVER);
    case 1:
        return (STATE_CLIENT);
    case 2:
        return (STATE_EXIT);
    default:
        break;
    }
    return (STATE_MENU);
}

static int menu_event(menu_t *menu, int exit_code)
{
    SDL_Event e;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            exit_code = STATE_EXIT;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                exit_code = STATE_EXIT;
                break;
            case SDLK_RETURN:
                exit_code = menu_enter(menu);
                break;
            case SDLK_UP:
                move_up(menu);
                break;
            case SDLK_DOWN:
                move_down(menu);
                break;
            default:
                break;
            }
        }
    }
    return (exit_code);
}

static void menu_destroy(menu_t *menu)
{
    if (menu != NULL) {
        if (menu->texture != NULL)
            SDL_DestroyTexture(menu->texture);
        for (int i = 0; i < 3; i++)
            if (menu->buttons[i] != NULL)
                button_destroy(menu->buttons[i]);
        free(menu);
    }
}

int menu_run(app_t *app)
{
    int exit_code = STATE_MENU;
    menu_t *menu = menu_create(app);

    if (menu == NULL)
        return (STATE_EXIT);
    while (exit_code == STATE_MENU) {
        menu_draw(app, menu);
        exit_code = menu_event(menu, exit_code);
        SDL_Delay(20);
    }
    menu_destroy(menu);
    return (exit_code);
}
