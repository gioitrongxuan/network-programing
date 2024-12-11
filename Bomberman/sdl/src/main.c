/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\sdl
** File description:
**      Main file
*/

#include "../inc/app.h"

#include <stdlib.h>

int main()
{
    const int tile_size = 32;
    game_t *game = game_init(25 * tile_size, 21 * tile_size, tile_size, 2);

    game_create_window(game);
    game->map = map_load(game->renderer, game->tile_size, "./rsc/map.txt");
    game->player1 = player_load(game->renderer, 1, 1, game->tile_size,
                                                        "./rsc/player.png");
    game->player1->bag = bag_load(game->renderer, game->tile_size,
                                    game->max_bombs, "./rsc/bomb.png");
    if (game->player1 == NULL || game->player1->bag == NULL) {
        game_destroy(game);
        return (EXIT_FAILURE);
    }
    app_run(game);
    game_destroy(game);
    return (EXIT_SUCCESS);
}
