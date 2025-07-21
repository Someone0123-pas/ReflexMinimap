#ifdef _WIN32

#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#endif

#include "error.h"
#include "log.h"
#include "socket.h"

static socket_t s_listening_socket = 0;
static socket_t s_active_socket = 0;

result_t socket_init() {
    socket_t listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    THROW_IFEQ(SOCKET_INIT_SOCKET, listening_socket, -1);
    LOG("socket_init(): Created socket with fd %d.", listening_socket);

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

result_t socket_close() {
    if (s_active_socket) {
        int close_res = close(s_active_socket);
        THROW_IFEQ(SOCKET_CLOSE_ACTIVE, close_res, -1);
        LOG("Active socket %d has been closed.", s_active_socket);
    }

    if (s_listening_socket) {
        int close_res = close(s_listening_socket);
        THROW_IFEQ(SOCKET_CLOSE_LISTENING, close_res, -1);
        LOG("Listening socket %d has been closed.", s_listening_socket);
    }

    return OK;
}
