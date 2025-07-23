#ifdef _WIN32

#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#endif

#include <stdlib.h>

#include "error.h"
#include "log.h"
#include "messageformat.h"
#include "socket.h"

#define NO_SOCKET (-1)

static socket_t s_listening_socket = NO_SOCKET;
static socket_t s_active_socket = NO_SOCKET;

[[nodiscard]] result_t socket_init() {
    socket_t listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    THROW_IFEQ(SOCKET_INIT_SOCKET, listening_socket, -1);
    LOG("socket_init(): Created socket with fd %d.", listening_socket);

    int reuseaddr = 1;
    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    THROW_IFEQ(SOCKET_INIT_SETSOCKOPT, listening_socket, -1);

    struct sockaddr_in socket_address = {
        .sin_family = AF_INET,
        .sin_port = htons(2346),
        .sin_addr = {.s_addr = htonl(INADDR_LOOPBACK)},
    };

    int bind_res = bind(listening_socket, (struct sockaddr*)&socket_address, sizeof(socket_address));
    THROW_IFEQ(SOCKET_INIT_BIND, bind_res, -1);
    LOG("socket_init(): Bound socket %d.", listening_socket);

    int listen_res = listen(listening_socket, 1);
    THROW_IFEQ(SOCKET_INIT_LISTEN, listen_res, -1);
    LOG("socket_init(): Socket %d is now listening.", listening_socket);

    s_listening_socket = listening_socket;
    LOG("socket_init(): Socket %d is registered in s_listening_socket.", listening_socket);
    return OK;
}

[[nodiscard]] result_t socket_connect() {
    THROW_IFEQ(SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET, s_listening_socket, NO_SOCKET);

    socket_t active_socket = accept(s_listening_socket, NULL, NULL);
    THROW_IFEQ(SOCKET_CONNECT_ACCEPT, active_socket, -1);
    LOG("socket_connect(): Socket %d is now connected with a client.", active_socket);

    s_active_socket = active_socket;
    LOG("socket_connect(): Socket %d is registered in s_active_socket.", active_socket);
    return OK;
}

[[nodiscard]] u8* socket_receive() {
    if (s_active_socket == NO_SOCKET) {
        WARN("socket_receive(): s_active_socket has not been set. Returning with NULL.");
        return NULL;
    }
    u8 header[2] = {};

    ssize_t recv_res = 0;
    recv_res = recv(s_active_socket, &header, sizeof(header), 0);
    switch (recv_res) {
    case sizeof(header):
        break;
    case 0: {
        LOG("socket_receive(): An orderly shutdown was detected. Returning with empty header.");
        u8* empty_header = calloc(2, sizeof(u8));
        return empty_header;
    } break;
    case -1: {
        WARN("socket_receive(): An error was detected. Returning NULL");
        return NULL;
    } break;
    default: {
        WARN("socket_receive(): Received bytes do not match specified bytes in header. Returning NULL.");
    } break;
    }

    // nmemb is chosen so that it is ensured that the header in front has place AND the string afterwards is zero-terminated
    u8* message = calloc(MSG_ARGS_SIZE(header) + 3, sizeof(u8));

    recv_res = recv(s_active_socket, message + 2, MSG_ARGS_SIZE(header), 0);
    switch (recv_res) {
    case 0: {
        LOG("socket_receive(): An orderly shutdown was detected. Returning with empty header.");
        free(message);
        u8* empty_header = calloc(2, sizeof(u8));
        return empty_header;
    } break;
    case -1: {
        WARN("socket_receive(): An error was detected. Returning NULL");
        free(message);
        return NULL;
    } break;
    default: {
        if (recv_res != MSG_ARGS_SIZE(header)) {
            WARN("socket_receive(): Received bytes do not match specified bytes in header. Returning NULL.");
            free(message);
            return NULL;
        }
    } break;
    }

    message[0] = header[0];
    message[1] = header[1];
    LOG("socket_receive(): Successfully received message.");
    return message;
}

result_t socket_close(bool close_only_active) {
    result_t res = OK;

    if (s_active_socket != NO_SOCKET) {
        int close_res = close(s_active_socket);
        if (close_res == -1) {
            WARN("socket_close(): Active socket %d could not be closed.", s_active_socket);
            res = SOCKET_CLOSE_ACTIVE;
        } else {
            LOG("socket_close(): Active socket %d has been closed.", s_active_socket);
        }
        s_active_socket = NO_SOCKET;
    }

    if (!close_only_active && s_listening_socket != NO_SOCKET) {
        int close_res = close(s_listening_socket);
        if (close_res == -1) {
            WARN("socket_close(): Listening socket %d could not be closed.", s_listening_socket);
            res = SOCKET_CLOSE_LISTENING;
        } else {
            LOG("socket_close(): Listening socket %d has been closed.", s_listening_socket);
        }
        s_listening_socket = NO_SOCKET;
    }

    return res;
}
