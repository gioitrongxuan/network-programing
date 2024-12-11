/*
** ETNA PROJECT, 10/02/2020 by habi_a
** c:\Users\habiy\Documents\bomberman\client_server
** File description:
**      State defintion file
*/

#ifndef _STATE_H_
#define _STATE_H_

#include "client.h"
#include "client_state.h"
#include "menu_state.h"
#include "server_state.h"
#include "server.h"

#define STATE_MENU 0
#define STATE_SERVER 1
#define STATE_CLIENT 2
#define STATE_SERVER_SOCKET 3
#define STATE_CLIENT_SOCKET 4
#define STATE_EXIT 5
#define STATE_NB 6

typedef int (*state_run_t)(app_t *);

#endif /* _STATE_H_ */
