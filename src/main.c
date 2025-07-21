#include <errno.h>
#include <stdio.h>

#include "error.h"
#include "socket.h"

#define EXIT_ERR(message) \
    ({                    \
        perror(message);  \
        return errno;     \
    })

#define NO_IMPL_EXIT(functionname) ({ fprintf(stderr, "%s returned an unknown result.\n", functionname); })

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

    result_t socket_close_res = socket_close();
    switch (socket_close_res) {
    case OK:
        break;
    case SOCKET_CLOSE_ACTIVE:
        EXIT_ERR("In socket_close(), with close(active_socket)");
    case SOCKET_CLOSE_LISTENING:
        EXIT_ERR("In socket_close(), with close(listening_socket)");
    default:
        NO_IMPL_EXIT("socket_close()");
    }

    return 0;
}