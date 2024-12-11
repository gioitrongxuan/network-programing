/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Explosion function file
*/

#include "../inc/action.h"
#include "../inc/explosion.h"
#include "../inc/explosion_queue.h"

static player_t *get_owner_bomb(game_t *game, bomb_t *bomb_to_check)
{
    bomb_t *bomb = NULL;

    for (int i = 0; i < NB_PLAYERS; i++) {
        bomb = game->players[i]->bag->first;
        while (bomb != NULL) {
            if (bomb == bomb_to_check)
                return (game->players[i]);
            bomb = bomb->next;
        }
    }
    return (NULL);
}

static int explose_bomb_top(app_t *app, game_t *game, bomb_t *bomb, int range
                            , int max)
{
    bomb_t *bomb_colat = NULL;
    block_t *block = NULL;
    SDL_Point coord = { bomb->coord.x, bomb->coord.y - (range + 1) };
    int is_drawable = 1;
    int stop = 0;

    if ((bomb_colat = get_bomb_active_here(game, &coord)) != NULL) {
        explose_bomb(app, game, bomb_colat, get_owner_bomb(game, bomb_colat)->power);
    } else if ((block = get_block_undestroyed_here(game, &coord)) != NULL) {
        if (block->block_type == BLOCK_NORMAL)
            block->is_destroyed = 1;
        else
            is_drawable = 0;
        stop = 1;
    }
    if (is_drawable) {
        if (range == max)
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_TOP
                        , "./rsc/sprites/bomb/explosion/top.png", app->renderer);
        else
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_VERT
                    , "./rsc/sprites/bomb/explosion/vert.png", app->renderer);
    }
    return (stop);
}

static int explose_bomb_bottom(app_t *app, game_t *game, bomb_t *bomb, int range
                            , int max)
{
    bomb_t *bomb_colat = NULL;
    block_t *block = NULL;
    SDL_Point coord = { bomb->coord.x, bomb->coord.y + (range + 1) };
    int is_drawable = 1;
    int stop = 0;

    if ((bomb_colat = get_bomb_active_here(game, &coord)) != NULL) {
        explose_bomb(app, game, bomb_colat, get_owner_bomb(game, bomb_colat)->power);
    } else if ((block = get_block_undestroyed_here(game, &coord)) != NULL) {
        if (block->block_type == BLOCK_NORMAL)
            block->is_destroyed = 1;
        else
            is_drawable = 0;
        stop = 1;
    }
    if (is_drawable) {
        if (range == max)
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_DOWN
                        , "./rsc/sprites/bomb/explosion/down.png", app->renderer);
        else
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_VERT
                    , "./rsc/sprites/bomb/explosion/vert.png", app->renderer);
    }
    return (stop);
}

static int explose_bomb_left(app_t *app, game_t *game, bomb_t *bomb, int range
                            , int max)
{
    bomb_t *bomb_colat = NULL;
    block_t *block = NULL;
    SDL_Point coord = { bomb->coord.x - (range + 1), bomb->coord.y };
    int is_drawable = 1;
    int stop = 0;

    if ((bomb_colat = get_bomb_active_here(game, &coord)) != NULL) {
        explose_bomb(app, game, bomb_colat, get_owner_bomb(game, bomb_colat)->power);
    } else if ((block = get_block_undestroyed_here(game, &coord)) != NULL) {
        if (block->block_type == BLOCK_NORMAL)
            block->is_destroyed = 1;
        else
            is_drawable = 0;
        stop = 1;
    }
    if (is_drawable) {
        if (range == max)
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_LEFT
                        , "./rsc/sprites/bomb/explosion/left.png", app->renderer);
        else
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_HORI
                    , "./rsc/sprites/bomb/explosion/hori.png", app->renderer);
    }
    return (stop);
}

static int explose_bomb_right(app_t *app, game_t *game, bomb_t *bomb, int range
                            , int max)
{
    bomb_t *bomb_colat = NULL;
    block_t *block = NULL;
    SDL_Point coord = { bomb->coord.x + (range + 1), bomb->coord.y };
    int is_drawable = 1;
    int stop = 0;

    if ((bomb_colat = get_bomb_active_here(game, &coord)) != NULL) {
        explose_bomb(app, game, bomb_colat, get_owner_bomb(game, bomb_colat)->power);
    } else if ((block = get_block_undestroyed_here(game, &coord)) != NULL) {
        if (block->block_type == BLOCK_NORMAL)
            block->is_destroyed = 1;
        else
            is_drawable = 0;
        stop = 1;
    }
    if (is_drawable) {
        if (range == max)
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_RIGHT
                        , "./rsc/sprites/bomb/explosion/right.png", app->renderer);
        else
            add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_HORI
                    , "./rsc/sprites/bomb/explosion/hori.png", app->renderer);
    }
    return (stop);
}

static int explose_bomb_center(app_t *app, game_t *game, bomb_t *bomb)
{
    player_t *player = NULL;
    SDL_Point coord = { bomb->coord.x, bomb->coord.y };

    add_explosion(game->explo_queue, &coord, app->tile_size, EXPLO_CENTER
                    , "./rsc/sprites/bomb/explosion/center.png", app->renderer);
    if ((player = get_player_alive_here(game, &coord)) != NULL) {
        player->is_alive = 0;
    }
    return (0);
}

void explose_bomb(app_t *app, game_t *game, bomb_t *bomb, int power)
{
    int top_stopped = 0;
    int down_stopped = 0;
    int left_stopped = 0;
    int right_stopped = 0;

    bomb->is_active = 0;
    explose_bomb_center(app, game, bomb);
    for (int range = 0; range < power; range++) {
        if (!top_stopped)
            top_stopped = explose_bomb_top(app, game, bomb, range, power -1);
        if (!down_stopped)
            down_stopped = explose_bomb_bottom(app, game, bomb, range, power -1);
        if (!left_stopped)
            left_stopped = explose_bomb_left(app, game, bomb, range, power -1);
        if (!right_stopped)
            right_stopped = explose_bomb_right(app, game, bomb, range, power -1);
    }
}
