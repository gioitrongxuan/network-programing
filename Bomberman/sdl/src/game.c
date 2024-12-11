/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Game function file
*/

#include "../inc/game.h"

#include <stdio.h>
#include <stdlib.h>

game_t *game_init(int screen_w, int screen_h, int tile_size, int max_bomb)
{
    game_t *game = malloc(sizeof(game_t));

    if (game == NULL) {
        fprintf(stderr, "Failed to malloc game");
        exit(EXIT_FAILURE);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to initialized the SDL: %s\n", SDL_GetError());
        game_destroy(game);
        exit(EXIT_FAILURE);
    }
    game->player1 = NULL;
    game->map = NULL;
    game->max_bombs = max_bomb;
    game->tile_size = tile_size;
    game->screen_size.x = screen_w;
    game->screen_size.y = screen_h;
    game->renderer = NULL;
    game->window = NULL;
    return (game);
}

void game_create_window(game_t *game)
{
    game->window = SDL_CreateWindow("Bomberman"
        , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
        , game->screen_size.x, game->screen_size.y, SDL_WINDOW_SHOWN);

    if (game->window == NULL) {
        fprintf(stderr, "Failed to create the window: %s\n", SDL_GetError());
        game_destroy(game);
        exit(EXIT_FAILURE);
    }
    game->renderer = SDL_CreateRenderer(game->window, -1,
                                        SDL_RENDERER_ACCELERATED);
    if (game->renderer == NULL) {
        fprintf(stderr, "Failed to create the renderer: %s\n", SDL_GetError());
        game_destroy(game);
        exit(EXIT_FAILURE);
    }
}

void game_destroy(game_t *game)
{
    if (game->player1 != NULL)
        player_destroy(game->player1);
    if (game->map != NULL)
        map_destroy(game->map);
    if (game != NULL) {
        if (game->renderer != NULL)
            SDL_DestroyRenderer(game->renderer);
        if (game->window != NULL)
            SDL_DestroyWindow(game->window);
    }
    SDL_Quit();
    if (game != NULL)
        free(game);
}

void game_draw(game_t *game)
{
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);
    map_draw(game->map, game->renderer, game->tile_size);
    player_draw(game->player1, game->renderer);
    SDL_RenderPresent(game->renderer);
}
