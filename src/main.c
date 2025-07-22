#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "error.h"
#include "log.h"
#include "socket.h"
#include "window.h"

char* g_errstr_custom = "";
char* g_errstr_errno = "";

bool g_logging_enabled = true;

#define ERR_TO_WIN(_main_res, message)   \
    ({                                   \
        g_errstr_errno = "";             \
        main_res = _main_res;            \
        g_errstr_custom = message;       \
        windowstate = WindowState_Error; \
        WARN(message);                   \
    })

#define ERRNO_TO_WIN(message)                            \
    ({                                                   \
        g_errstr_errno = strerror(errno);                \
        main_res = errno;                                \
        g_errstr_custom = message;                       \
        windowstate = WindowState_Error;                 \
        WARN("%s: %s", g_errstr_custom, g_errstr_errno); \
    })

#define NO_IMPL_TO_WIN(functionname)                                   \
    ({                                                                 \
        g_errstr_errno = "";                                           \
        main_res = 1;                                                  \
        g_errstr_custom = functionname " returned an unknown result."; \
        windowstate = WindowState_Error;                               \
        WARN("%s returned an unknown result.", functionname);          \
    })

struct Args_connect_init {
    int m_main_res;
    enum WindowState m_windowstate;
};

int connect_init(void* args) {
    int res = OK;
    int main_res = 1;
    enum WindowState windowstate = WindowState_Error;

    result_t socket_connect_res = socket_connect();
    switch (socket_connect_res) {
    case OK:
        res = OK;
        main_res = 0;
        windowstate = WindowState_Connected;
        LOG("connect_init(): Thread connect_init() successfully established a connection.");
        break;
    case SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET:
        res = 1;
        g_errstr_errno = "";
        main_res = 1;
        g_errstr_custom = "Attempted to call socket_connect() without setting listening socket! Was socket_init() already called?";
        windowstate = WindowState_Error;
        WARN("Thread connect_init() called socket_init(), without s_listening_socket being set.");
        break;
    case SOCKET_CONNECT_ACCEPT:
        res = 2;
        ERRNO_TO_WIN("In socket_connect(), with accept()");
    default:
        res = -1;
        NO_IMPL_TO_WIN("socket_connect()");
    }

    ((struct Args_connect_init*)args)->m_main_res = main_res;
    ((struct Args_connect_init*)args)->m_windowstate = windowstate;
    LOG("connect_init(): Written %d into main_res and %d into windowstate.", main_res, windowstate);

    return res;
}

int main(void) {
    int main_res = 0;
    enum WindowState windowstate = WindowState_AttemptConnection;

    result_t socket_init_res = socket_init();
    switch (socket_init_res) {
    case OK:
        break;
    case SOCKET_INIT_SOCKET:
        ERRNO_TO_WIN("In socket_init(), with socket()");
        break;
    case SOCKET_INIT_BIND:
        ERRNO_TO_WIN("In socket_init(), with bind()");
        break;
    case SOCKET_INIT_LISTEN:
        ERRNO_TO_WIN("In socket_init(), with listen()");
        break;
    case SOCKET_INIT_SETSOCKOPT:
        ERRNO_TO_WIN("In socket_init(), with setsockopt()");
        break;
    default:
        NO_IMPL_TO_WIN("socket_init()");
        break;
    }

    InitWindow(800, 400, "KATAM-Minimap");
    thrd_t connect_init_thread = 0;
    struct Args_connect_init connect_init_args = {.m_windowstate = WindowState_WaitForConnection, .m_main_res = main_res};

    while (!WindowShouldClose()) {
        window_draw_func draw = DRAW_NONE;

        switch (windowstate) {

        case WindowState_AttemptConnection: {
            draw = draw_try_connect;
            int thrd_create_res = thrd_create(&connect_init_thread, connect_init, &connect_init_args);
            if (thrd_create_res != thrd_success) {
                ERR_TO_WIN(1, "Failed to create a thread for accepting a client.");
                break;
            }
            LOG("main(): Thread for accepting client successfully created.");
            windowstate = WindowState_WaitForConnection;
        } break;

        case WindowState_WaitForConnection: {
            draw = draw_try_connect;
            windowstate = connect_init_args.m_windowstate;
            main_res = connect_init_args.m_main_res;
        } break;

        case WindowState_Connected: {
            draw = draw_connected;
        } break;

        case WindowState_Error: {
            draw = draw_error;
        } break;
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