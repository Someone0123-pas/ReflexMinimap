#include <stdio.h>

#include "network.h"
#include "types.h"

#define EXIT_IF_ERROR(result, message) \
    ({                                 \
        if ((result) != OK) {          \
            perror(message);           \
            return result;             \
        }                              \
    })

int main() {
    result_t socket_init_res = socket_init();
    EXIT_IF_ERROR(socket_init_res, "In socket_init()");

    result_t socket_close_res = socket_close();
    EXIT_IF_ERROR(socket_close_res, "In socket_close()");

    return 0;
}