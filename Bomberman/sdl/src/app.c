/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      App function file
*/

#include "../inc/action.h"
#include "../inc/app.h"
#include "../inc/move.h"

static void app_event_keyboard(game_t *game, SDL_Event *e, int *result)
{
    switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
        *result = 0;
        break;
    case SDLK_UP:
    case SDLK_DOWN:
    case SDLK_LEFT:
    case SDLK_RIGHT:
        move(game, e->key.keysym.sym);
        break;
    case SDLK_SPACE:
        action(game);
        break;
    default:
        break;
    }
}

static int app_event(game_t *game)
{
    SDL_Event e;
    int result = 1;

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            result = 0;
        else if (e.type == SDL_KEYDOWN) {
            app_event_keyboard(game, &e, &result);
        }
    }
    return (result);
}

void app_run(game_t *game)
{
    int game_is_running = 1;

    while (game_is_running) {
        game_draw(game);
        game_is_running = app_event(game);
        SDL_Delay(20);
    }
}
