#ifndef SOCKET_H
#define SOCKET_H

#include "error.h"

/*
 * Initialises socket to be listening.
 */
result_t socket_init();
#define SOCKET_INIT_SOCKET 1
#define SOCKET_INIT_BIND 2
#define SOCKET_INIT_LISTEN 3

/*
 * Closes all sockets.
 */
result_t socket_close();
#define SOCKET_CLOSE_ACTIVE 1
#define SOCKET_CLOSE_LISTENING 2

#endif  // SOCKET_H
