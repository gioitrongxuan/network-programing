/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
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

    surface = IMG_Load("./rsc/gate.png");
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

static server_state_t *server_state_create(app_t *app)
{
    server_state_t *server_state = malloc(sizeof(server_state_t));
    SDL_Rect button_pos1 = { 1.5 * app->tile_size, 10 * app->tile_size, 2.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos2 = { 1.5 * app->tile_size, 12 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos3 = { 1.5 * app->tile_size, 19.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };
    SDL_Rect button_pos4 = { 9.25 * app->tile_size, 19.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };

    if (server_state == NULL) {
        fprintf(stderr, "Failed to malloc server_state\n");
        return (NULL);
    }
    server_state->texture = server_state_background(app);
    if (server_state->texture == NULL)
        return(NULL);
    server_state->buttons[0] = button_create(app, "Port:", button_pos1);
    server_state->buttons[1] = button_create(app, " ", button_pos2);
    server_state->buttons[2] = button_create(app, "Echap: Retour", button_pos3);
    server_state->buttons[3] = button_create(app, "Entree: Suivant", button_pos4);
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
    for (int i = 0; i < 4; i++)
        button_draw(app, server_state->buttons[i]);
    SDL_RenderPresent(app->renderer);
}

static void move_down(server_state_t *server_state)
{
    if (server_state->index_select < 1) {
        server_state->buttons[server_state->index_select++]->selected = 0;
        server_state->buttons[server_state->index_select]->selected = 1;
    }
}

static int server_state_enter(app_t *app, server_state_t *server_state, char input_text[17])
{
    switch (server_state->index_select) {
    case 0:
        server_state->index_select++;
        app->port = SDL_atoi(input_text);
        move_down(server_state);
        return (STATE_SERVER_SOCKET);
    default:
        break;
    }
    return (STATE_SERVER);
}

static int server_state_event(app_t *app, server_state_t *server_state, char input_text[17], int *len)
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
                    server_state->buttons[1]->position.w -= 0.5 * app->tile_size;
                }
                break;
            case SDLK_ESCAPE:
                exit_code = STATE_MENU;
                break;
            case SDLK_RETURN:
                exit_code = server_state_enter(app, server_state, input_text);
                break;
            default:
                break;
            }
        } else if (e.type == SDL_TEXTINPUT) {
            if (*len < 5) {
                input_text[*len] = e.text.text[0];
                *len += 1;
                server_state->buttons[1]->position.w += 0.5 * app->tile_size;
                change_made = 1;
            }
        }
    }
    if (change_made) {
        if (server_state->buttons[1]->texture_normal != NULL)
            SDL_DestroyTexture(server_state->buttons[1]->texture_normal);
        if (!SDL_strcmp(input_text, ""))
            server_state->buttons[1]->texture_normal
                        = load_text_texture(app, " ", &color);
        else
            server_state->buttons[1]->texture_normal
                        = load_text_texture(app, input_text, &color);
    }
    return (exit_code);
}

static void server_state_destroy(server_state_t *server_state)
{
    if (server_state != NULL) {
        if (server_state->texture != NULL)
            SDL_DestroyTexture(server_state->texture);
        for (int i = 0; i < 4; i++)
            if (server_state->buttons[i] != NULL)
                button_destroy(server_state->buttons[i]);
        free(server_state);
    }
}

int server_state_run(app_t *app)
{
    int len = 0;
    int exit_code = STATE_SERVER;
    char port_text[17] = { 0 };
    server_state_t *server_state = server_state_create(app);

    if (server_state == NULL)
        return (STATE_EXIT);
    SDL_StartTextInput();
    while (exit_code == STATE_SERVER) {
        server_state_draw(app, server_state);
        if (!server_state->index_select)
            exit_code = server_state_event(app, server_state, port_text, &len);
        SDL_Delay(20);
    }
    SDL_StopTextInput();
    server_state_destroy(server_state);
    return (exit_code);
}
