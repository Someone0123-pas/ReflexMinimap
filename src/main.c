#include <raylib.h>
#include <stdlib.h>
#include <threads.h>

#include "error.h"
#include "socket.h"
#include "window.h"

char* g_errorstr[0x100] = {};

#define EXIT_ERR(message)      \
    ({                         \
        perror(message);       \
        int cur_errno = errno; \
        socket_close();        \
        return cur_errno;      \
    })

result_t connect_to_client() {
    result_t socket_connect_res = socket_connect();
    switch (socket_connect_res) {
    case OK:
        return OK;
    case SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET:
        fprintf(stderr, "Attempted to call socket_connect() without setting listening socket! Was socket_init() already called?");
        return 2;
    case SOCKET_CONNECT_ACCEPT:
        EXIT_ERR("In socket_connect(), with accept()");
    default:
        NO_IMPL_EXIT("socket_connect()");
    }
}

int main(void) {
    int main_res = 0;
    enum WindowState windowstate = WindowState_AttemptConnection;

    // TODO: Rewrite EXIT_ERR Macro to set main_res and g_errorstr -> ERR_TO_WIN
    // Also Rewrite NO_IMPL_EXIT

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
    case SOCKET_INIT_SETSOCKOPT:
        EXIT_ERR("In socket_init(), with setsockopt()");
    default:
        NO_IMPL_EXIT("socket_init()");
    }

    InitWindow(300, 300, "KATAM-Minimap");

    while (!WindowShouldClose()) {
        window_draw_func draw = DRAW_NONE;

        switch (windowstate) {
        case WindowState_AttemptConnection:
            draw = draw_try_connect;
            // TODO: Call connect_to_client as thread
            break;
        case WindowState_WaitForConnection:
            // TODO: Wait for connect_to_client-thread to finish
            break;
        case WindowState_Connected:
            break;
        case WindowState_Error:
            draw = draw_error;
            break;
        }

        BeginDrawing();
        ClearBackground(PINK);

        if (draw != DRAW_NONE) {
            draw();
        }

        EndDrawing();
    }

    CloseWindow();
    socket_close();
    return main_res;
}