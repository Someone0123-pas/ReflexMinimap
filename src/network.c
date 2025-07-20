#ifdef _WIN32

#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#endif

#include "network.h"
#include "types.h"

static socket_t s_listening_socket = 0;
static socket_t s_active_socket = 0;

result_t socket_init() {
    socket_t listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    THROW_ERRNO_IFEQ(listening_socket, -1);

    struct sockaddr_in socket_address = {
        .sin_family = AF_INET,
        .sin_port = htons(2346),
        .sin_addr = {.s_addr = htonl(INADDR_LOOPBACK)},
    };

    int bind_res = bind(listening_socket, (struct sockaddr*)&socket_address, sizeof(socket_address));
    THROW_ERRNO_IFEQ(bind_res, -1);

    int listen_res = listen(listening_socket, 1);
    THROW_ERRNO_IFEQ(listen_res, -1);

    s_listening_socket = listening_socket;
    return OK;
}

result_t socket_close() {
    if (s_active_socket) {
        int close_res = close(s_active_socket);
        THROW_ERRNO_IFEQ(close_res, -1);
    }
    if (s_listening_socket) {
        int close_res = close(s_listening_socket);
        THROW_ERRNO_IFEQ(close_res, -1);
    }

    return OK;
}
