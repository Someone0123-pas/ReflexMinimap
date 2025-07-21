#include "error.h"
#include "socket.h"

int main() {
    result_t socket_init_res = socket_init();
    switch (socket_init_res) {
    case OK:
        break;
    case SOCKET_INIT_SOCKET:
        EXIT_ERR("In socket_init(), with socket()");
    case SOCKET_INIT_BIND:
        EXIT_ERR("In socket_init(), with bind()");
    case SOCKET_INIT_LISTEN:
        EXIT_ERR("In socket_init(), with listen()");
    default:
        NO_IMPL_EXIT("socket_init()");
    }

    result_t socket_connect_res = socket_connect();
    switch (socket_connect_res) {
    case OK:
        break;
    case SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET:
        fprintf(stderr, "Attempted to call socket_connect() without setting listening socket! Was socket_init() already called?");
        return 1;
    case SOCKET_CONNECT_ACCEPT:
        EXIT_ERR("In socket_connect(), with accept()");
    default:
        NO_IMPL_EXIT("socket_connect()");
    }

    socket_close();
    return 0;
}