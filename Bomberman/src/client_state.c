/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman
** File description:
**      Client state function file
*/

#include "../inc/app.h"
#include "../inc/client.h"
#include "../inc/client_state.h"
#include "../inc/state.h"

#include <stdio.h>
#include <stdlib.h>

static SDL_Texture *client_state_background(app_t *app)
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

static client_state_t *client_state_create(app_t *app)
{
    client_state_t *client_state = malloc(sizeof(client_state_t));
    SDL_Rect button_pos1 = { 1.5 * app->tile_size, 11 * app->tile_size, 5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos2 = { 1.5 * app->tile_size, 13 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos3 = { 1.5 * app->tile_size, 15 * app->tile_size, 2.5 * app->tile_size, 1.1 * app->tile_size };
    SDL_Rect button_pos4 = { 1.5 * app->tile_size, 17 * app->tile_size, 0 * app->tile_size, 0.8 * app->tile_size };
    SDL_Rect button_pos5 = { 1.5 * app->tile_size, 20.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };
    SDL_Rect button_pos6 = { 9.25 * app->tile_size, 20.5 * app->tile_size, 3 * app->tile_size, 1 * app->tile_size };

    if (client_state == NULL) {
        fprintf(stderr, "Failed to malloc client_state\n");
        return (NULL);
    }
    client_state->texture = client_state_background(app);
    if (client_state->texture == NULL)
        return(NULL);
    client_state->buttons[0] = button_create("IP Serveur:", button_pos1, app->renderer, app->font);
    client_state->buttons[1] = button_create(" ", button_pos2, app->renderer, app->font);
    client_state->buttons[2] = button_create("Port:", button_pos3, app->renderer, app->font);
    client_state->buttons[3] = button_create(" ", button_pos4, app->renderer, app->font);
    client_state->buttons[4] = button_create("Echap: Retour", button_pos5, app->renderer, app->font);
    client_state->buttons[5] = button_create("Entree: Suivant", button_pos6, app->renderer, app->font);
    client_state->index_select = 0;
    client_state->buttons[client_state->index_select]->selected = 1;
    return (client_state);
}

static void client_state_draw(app_t *app, client_state_t *client_state)
{
    SDL_Rect full_screen = { 0, 0, app->screen_size.x, app->screen_size.y };

    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
    SDL_RenderCopy(app->renderer, client_state->texture, NULL, &full_screen);
    for (int i = 0; i < 6; i++)
        button_draw(client_state->buttons[i], app->renderer);
    SDL_RenderPresent(app->renderer);
}

static void move_down(client_state_t *client_state)
{
    if (client_state->index_select < 1) {
        client_state->buttons[client_state->index_select]->selected = 0;
        client_state->index_select += 2;
        client_state->buttons[client_state->index_select]->selected = 1;
    }
}

static int client_state_enter(app_t *app, client_state_t *client_state, char input_text[17], int *len)
{
    switch (client_state->index_select) {
    case 0:
        app->ip = input_text;
        move_down(client_state);
        *len = 0;
        break;
    case 2:
        client_state->index_select++;
        app->port = SDL_atoi(input_text);
        return (STATE_CLIENT_SOCKET);
    default:
        break;
    }
    return (STATE_CLIENT);
}

static int client_state_event(app_t *app, client_state_t *client_state
                                , char input_text[17], int *len, int max)
{
    int change_made = 0;
    int exit_code = STATE_CLIENT;
    SDL_Event e;
    SDL_Color color = { 255, 255, 255, 255 };

    if (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit_code = STATE_EXIT;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym)
            {
            case SDLK_BACKSPACE:
                if (*len) {
                    if (input_text[*len - 1] == '.')
                        client_state->buttons[client_state->index_select + 1]->position.w -= 0.25 * app->tile_size;
                    else
                        client_state->buttons[client_state->index_select + 1]->position.w -= 0.5 * app->tile_size;
                    input_text[*len - 1] = 0;
                    *len -= 1;
                    change_made = 1;
                }
                break;
            case SDLK_ESCAPE:
                exit_code = STATE_MENU;
                break;
            case SDLK_RETURN:
                exit_code = client_state_enter(app, client_state, input_text, len);
                break;
            default:
                break;
            }
        }
        else if (e.type == SDL_TEXTINPUT) {
            if (*len < max) {
                input_text[*len] = e.text.text[0];
                *len += 1;
                if (e.text.text[0] == '.')
                    client_state->buttons[client_state->index_select + 1]->position.w += 0.25 * app->tile_size;
                else
                    client_state->buttons[client_state->index_select + 1]->position.w += 0.5 * app->tile_size;
                change_made = 1;
            }
        }
    }
    if (change_made) {
        if (client_state->buttons[client_state->index_select + 1]->texture_normal != NULL)
            SDL_DestroyTexture(client_state->buttons[client_state->index_select + 1]->texture_normal);
        if (!SDL_strcmp(input_text, ""))
            client_state->buttons[client_state->index_select + 1]->texture_normal
            = load_text_texture(" ", &color, app->renderer, app->font);
        else
            client_state->buttons[client_state->index_select + 1]->texture_normal
            = load_text_texture(input_text, &color, app->renderer, app->font);
    }
    return (exit_code);
}

static void client_state_destroy(client_state_t *client_state)
{
    if (client_state != NULL) {
        if (client_state->texture != NULL)
            SDL_DestroyTexture(client_state->texture);
        for (int i = 0; i < 6; i++)
            if (client_state->buttons[i] != NULL)
                button_destroy(client_state->buttons[i]);
        free(client_state);
    }
}

int client_state_run(app_t *app)
{
    int len = 0;
    int exit_code = STATE_CLIENT;
    char ip_adress[17] = { 0 };
    char port_text[17] = { 0 };
    client_state_t *client_state = client_state_create(app);

    if (client_state == NULL)
        return (STATE_EXIT);
    SDL_StartTextInput();
    while (exit_code == STATE_CLIENT) {
        client_state_draw(app, client_state);
        if (!client_state->index_select)
            exit_code = client_state_event(app, client_state, ip_adress, &len, 15);
        else
            exit_code = client_state_event(app, client_state, port_text, &len, 5);
        SDL_Delay(20);
    }
    SDL_StopTextInput();
    client_state_destroy(client_state);
    return (exit_code);
}
