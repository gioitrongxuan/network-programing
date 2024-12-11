/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      App function file
*/

#include "../inc/app.h"

#include <stdio.h>
#include <stdlib.h>

static app_t *app_init(void)
{
    app_t *app = malloc(sizeof(app_t));

    if (app == NULL) {
        fprintf(stderr, "Failed to malloc app\n");
        exit(EXIT_FAILURE);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to initialized the SDL: %s\n", SDL_GetError());
        app_destroy(app);
        exit(EXIT_FAILURE);
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "Failed to initialized the SDL_TTF: %s\n", TTF_GetError());
        app_destroy(app);
        exit(EXIT_FAILURE);
    }
    return (app);
}

static void app_fill(app_t *app, int screenw, int screenh, int tilesize)
{
    app->time_left = 0;
    app->port = 0;
    app->ip = NULL;
    app->index_map = 1;
    app->maps_available[0] = "./rsc/maps/map1.txt";
    app->maps_available[1] = "./rsc/maps/map2.txt";
    app->nb_bomb_start = 1;
    app->font = NULL;
    app->renderer = NULL;
    app->window = NULL;
    app->tile_size = tilesize;
    app->screen_size.x = screenw;
    app->screen_size.y = screenh;
}

app_t *app_start(int screen_w, int screen_h, int tile_size)
{
    app_t *app = app_init();

    app_fill(app,screen_w, screen_h, tile_size);
    app->window = SDL_CreateWindow("Bomberman"
        , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , app->screen_size.x, app->screen_size.y, SDL_WINDOW_SHOWN);
    if (app->window == NULL) {
        fprintf(stderr, "Failed to create the window: %s\n", SDL_GetError());
        app_destroy(app);
        exit(EXIT_FAILURE);
    }
    app->renderer = SDL_CreateRenderer(app->window, -1,
                                        SDL_RENDERER_ACCELERATED);
    if (app->renderer == NULL) {
        fprintf(stderr, "Failed to create the renderer: %s\n", SDL_GetError());
        app_destroy(app);
        exit(EXIT_FAILURE);
    }
    app->font = TTF_OpenFont("./rsc/fonts/bomberman.ttf", 24);
    if (app->font == NULL) {
        fprintf(stderr, "Failed to load ttf: %s\n", TTF_GetError());
        app_destroy(app);
        exit(EXIT_FAILURE);
    }
    return (app);
}

void app_destroy(app_t *app)
{
    if (app != NULL) {
        if (app->renderer != NULL)
            SDL_DestroyRenderer(app->renderer);
        if (app->window != NULL)
            SDL_DestroyWindow(app->window);
        if (app->font != NULL)
            TTF_CloseFont(app->font);
        TTF_Quit();
        SDL_Quit();
        free(app);
    }
}
