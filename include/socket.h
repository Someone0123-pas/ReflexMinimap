#ifndef SOCKET_H
#define SOCKET_H

#include "error.h"
#include "types.h"

/*
 * Initialises socket to be listening.
 */
result_t socket_init();
#define SOCKET_INIT_SOCKET 1
#define SOCKET_INIT_BIND 2
#define SOCKET_INIT_LISTEN 3
#define SOCKET_INIT_SETSOCKOPT 4

/*
 * Blocks until a client wants to connect.
 */
result_t socket_connect();
#define SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET 1
#define SOCKET_CONNECT_ACCEPT 2

/*
 * Blocks until two messages (specified in messageformat.h) are received from a client.
 * If an orderly shutdown was detected, an empty header (0x00, 0x00) is returned.
 * The message is allocated on the heap and must be freed after use!
 * Except when an error occured or the received bytes don't match, where NULL is returned.
 */
u8* socket_receive();

/*
 * Attempts to close all sockets, regardless of errors.
 * The errorcode indicates the last socket where errors occured.
 */
result_t socket_close(bool close_only_active);
#define SOCKET_CLOSE_ACTIVE 1
#define SOCKET_CLOSE_LISTENING 2

#endif  // SOCKET_H
