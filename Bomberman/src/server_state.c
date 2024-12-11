/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Server function file
*/

#include "../inc/app.h"
#include "../inc/server_state.h"
#include "../inc/server.h"
#include "../inc/state.h"

#include <stdio.h>
#include <stdlib.h>

static SDL_Texture *server_state_background(app_t *app)
{
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = NULL;

    surface = IMG_Load("./rsc/backgrounds/gate.png");
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

static void server_state_buttons(app_t *app, server_state_t *server_state)
{
    SDL_Rect button_pos1 = { 1.5 * app->tile_size, 9 * app->tile_size, 6 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos2 = { 1.5 * app->tile_size, 10 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos3 = { 1.5 * app->tile_size, 11.5 * app->tile_size, 8.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos4 = { 1.5 * app->tile_size, 12.5 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos5 = { 1.5 * app->tile_size, 14 * app->tile_size, 10.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos6 = { 1.5 * app->tile_size, 15 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos7 = { 1.5 * app->tile_size, 16.5 * app->tile_size, 2.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos8 = { 1.5 * app->tile_size, 17.5 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos9 = { 1.5 * app->tile_size, 20.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };
    SDL_Rect button_pos10 = { 9.25 * app->tile_size, 20.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };

    server_state->buttons[0] = button_create("Index de la map:", button_pos1, app->renderer, app->font);
    server_state->buttons[1] = button_create(" ", button_pos2, app->renderer, app->font);
    server_state->buttons[2] = button_create("Temps max en minutes:", button_pos3, app->renderer, app->font);
    server_state->buttons[3] = button_create(" ", button_pos4, app->renderer, app->font);
    server_state->buttons[4] = button_create("Nombre de bombe au demarrage:", button_pos5, app->renderer, app->font);
    server_state->buttons[5] = button_create(" ", button_pos6, app->renderer, app->font);
    server_state->buttons[6] = button_create("Port:", button_pos7, app->renderer, app->font);
    server_state->buttons[7] = button_create(" ", button_pos8, app->renderer, app->font);
    server_state->buttons[8] = button_create("Echap: Retour", button_pos9, app->renderer, app->font);
    server_state->buttons[9] = button_create("Entree: Suivant", button_pos10, app->renderer, app->font);
}

static server_state_t *server_state_create(app_t *app)
{
    server_state_t *server_state = malloc(sizeof(server_state_t));

    if (server_state == NULL) {
        fprintf(stderr, "Failed to malloc server_state\n");
        return (NULL);
    }
    server_state->texture = server_state_background(app);
    if (server_state->texture == NULL)
        return(NULL);
    server_state_buttons(app, server_state);
    server_state->index_select = 0;
    server_state->buttons[server_state->index_select]->selected = 1;
    return (server_state);
}

static void server_state_draw(app_t *app, server_state_t *server_state)
{
    SDL_Rect full_screen = { 0, 0, app->screen_size.x, app->screen_size.y };

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderCopy(app->renderer, server_state->texture, NULL, &full_screen);
    for (int i = 0; i < 10; i++)
        button_draw(server_state->buttons[i], app->renderer);
    SDL_RenderPresent(app->renderer);
}

static void move_down(server_state_t *server_state)
{
    server_state->buttons[server_state->index_select]->selected = 0;
    server_state->index_select += 2;
    server_state->buttons[server_state->index_select]->selected = 1;
}

static int server_state_enter(app_t *app, server_state_t *server_state
                                , char input_text[8], int *len)
{
    switch (server_state->index_select) {
    case 0:
        app->index_map = SDL_atoi(input_text);
        if (app->index_map && app->index_map <= NB_MAP) {
            move_down(server_state);
            *len = 0;
        }
        break;
    case 2:
        app->time_left = SDL_atoi(input_text);
        move_down(server_state);
        *len = 0;
        break;
    case 4:
        app->nb_bomb_start = SDL_atoi(input_text);
        if (app->nb_bomb_start) {
            move_down(server_state);
            *len = 0;
        }
        break;
    case 6:
        app->port = SDL_atoi(input_text);
        move_down(server_state);
        return (STATE_SERVER_SOCKET);
    }
    return (STATE_SERVER);
}

static int server_state_event(app_t *app, server_state_t *server_state
                                , char input_text[8], int *len, int max)
{
    int change_made = 0;
    int exit_code = STATE_SERVER;
    SDL_Event e;
    SDL_Color color = { 255, 255, 255, 255 };

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit_code = STATE_EXIT;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym)
            {
            case SDLK_BACKSPACE:
                if (*len) {
                    input_text[*len - 1] = 0;
                    *len -= 1;
                    change_made = 1;
                    server_state->buttons[server_state->index_select + 1]->position.w -= 0.5 * app->tile_size;
                }
                break;
            case SDLK_ESCAPE:
                exit_code = STATE_MENU;
                break;
            case SDLK_RETURN:
                exit_code = server_state_enter(app, server_state, input_text
                                                                        , len);
                break;
            default:
                break;
            }
        } else if (e.type == SDL_TEXTINPUT) {
            if (*len < max) {
                input_text[*len] = e.text.text[0];
                *len += 1;
                server_state->buttons[server_state->index_select + 1]->position.w += 0.5 * app->tile_size;
                change_made = 1;
            }
        }
    }
    if (change_made) {
        if (server_state->buttons[server_state->index_select + 1]->texture_normal != NULL)
            SDL_DestroyTexture(server_state->buttons[server_state->index_select + 1]->texture_normal);
        if (!SDL_strcmp(input_text, ""))
            server_state->buttons[server_state->index_select + 1]->texture_normal
                        = load_text_texture(" ", &color, app->renderer, app->font);
        else
            server_state->buttons[server_state->index_select + 1]->texture_normal
                        = load_text_texture(input_text, &color, app->renderer, app->font);
    }
    return (exit_code);
}

static void server_state_destroy(server_state_t *server_state)
{
    if (server_state != NULL) {
        if (server_state->texture != NULL)
            SDL_DestroyTexture(server_state->texture);
        for (int i = 0; i < 10; i++)
            if (server_state->buttons[i] != NULL)
                button_destroy(server_state->buttons[i]);
        free(server_state);
    }
}

int server_state_run(app_t *app)
{
    int len = 0;
    int exit_code = STATE_SERVER;
    char port[8] = { 0 };
    char id_map[8] = { 0 };
    char nb_bombs[8] = { 0 };
    char time_left[8] = { 0 };
    server_state_t *server_state = server_state_create(app);

    if (server_state == NULL)
        return (STATE_EXIT);
    SDL_StartTextInput();
    while (exit_code == STATE_SERVER) {
        server_state_draw(app, server_state);
        if (!server_state->index_select)
            exit_code = server_state_event(app, server_state, id_map, &len, 1);
        else if (server_state->index_select == 2)
            exit_code = server_state_event(app, server_state, time_left, &len, 3);
        else if (server_state->index_select == 4)
            exit_code = server_state_event(app, server_state, nb_bombs, &len, 1);
        else if (server_state->index_select == 6)
            exit_code = server_state_event(app, server_state, port, &len, 5);
        SDL_Delay(20);
    }
    SDL_StopTextInput();
    server_state_destroy(server_state);
    return (exit_code);
}
