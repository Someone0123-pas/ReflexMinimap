#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

#include "error.h"
#include "log.h"
#include "socket.h"
#include "window.h"

// ERRORS AND LOGS

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

#define NO_IMPL_TO_WIN(functionname)                                    \
    ({                                                                  \
        g_errstr_errno = "";                                            \
        main_res = 1;                                                   \
        g_errstr_custom = functionname "\nreturned an unknown result."; \
        windowstate = WindowState_Error;                                \
        WARN("%s returned an unknown result.", functionname);           \
    })

// THREADS

struct Thread {
    bool m_active;
    thrd_t m_thread;
};

/*
 * m_windowstate must always be written last, such that reading it first prevents race conditions.
 */
struct ThreadArgs {
    int m_main_res;
    enum WindowState m_windowstate;
};

typedef int thread_func;
#define EMPTY_THREAD {.m_active = false}

#define THREAD_PREAMBLE                                         \
    struct ThreadArgs* args = (struct ThreadArgs*)args_voidptr; \
    enum WindowState initial_windowstate = args->m_windowstate; \
    int main_res = 1;                                           \
    enum WindowState windowstate = WindowState_Error;

#define THREAD_EPILOGUE(functionname)                                                                                                                \
    ({                                                                                                                                               \
        enum WindowState current_windowstate = args->m_windowstate;                                                                                  \
        if (current_windowstate != initial_windowstate) {                                                                                            \
            WARN("Thread %s: During execution, the windowstate has changed from %d to %d.", functionname, initial_windowstate, current_windowstate); \
        }                                                                                                                                            \
        if (args->m_windowstate == WindowState_Error) {                                                                                              \
            WARN("Thread %s: The windowstate has changed to WindowState_Error externally, so further execution is aborted.", functionname);          \
            return 0;                                                                                                                                \
        }                                                                                                                                            \
        args->m_main_res = main_res;                                                                                                                 \
        args->m_windowstate = windowstate;                                                                                                           \
        LOG("Thread %s: Written %d into main_res and %d into windowstate.", functionname, main_res, windowstate);                                    \
        return 0;                                                                                                                                    \
    })

thread_func client_connect(void* args_voidptr) {
    THREAD_PREAMBLE;

    result_t socket_connect_res = socket_connect();
    switch (socket_connect_res) {
    case OK: {
        main_res = 0;
        windowstate = WindowState_Connected;
        LOG("Thread client_connect(): Thread client_connect() successfully established a connection.");
    } break;
    case SOCKET_CONNECT_LISTENING_SOCKET_NOT_SET: {
        g_errstr_errno = "";
        main_res = 1;
        g_errstr_custom = "Attempted to call socket_connect()\nwithout setting listening socket!\nWas socket_init() already called?";
        windowstate = WindowState_Error;
        WARN("Thread client_connect(): socket_init() called, without s_listening_socket being set.");
    } break;
    case SOCKET_CONNECT_ACCEPT: {
        ERRNO_TO_WIN("In socket_connect(), with accept()");
    } break;
    default: {
        NO_IMPL_TO_WIN("socket_connect()");
    } break;
    }

    THREAD_EPILOGUE("client_connect()");
}

thread_func client_receive(void* args_voidptr) {
    THREAD_PREAMBLE;

    // TODO

    THREAD_EPILOGUE("client_receive()");
}

// MAIN

constexpr double CONNECTED_WAIT_SECS = 2;

int main(void) {
    int main_res = 0;
    enum WindowState windowstate = WindowState_AttemptConnection;

    result_t socket_init_res = socket_init();
    switch (socket_init_res) {
    case OK:
        break;
    case SOCKET_INIT_SOCKET: {
        ERRNO_TO_WIN("In socket_init(), with socket()");
    } break;
    case SOCKET_INIT_BIND: {
        ERRNO_TO_WIN("In socket_init(), with bind()");
    } break;
    case SOCKET_INIT_LISTEN: {
        ERRNO_TO_WIN("In socket_init(), with listen()");
    } break;
    case SOCKET_INIT_SETSOCKOPT: {
        ERRNO_TO_WIN("In socket_init(), with setsockopt()");
    } break;
    default: {
        NO_IMPL_TO_WIN("socket_init()");
    } break;
    }

    InitWindow(400, 400, "KATAM-Minimap");
    struct Thread client_connect_thread = EMPTY_THREAD;
    struct ThreadArgs client_connect_args = {.m_windowstate = WindowState_AttemptConnection, .m_main_res = main_res};
    /*
    struct Thread client_receive_thread = EMPTY_THREAD;
    struct ThreadArgs client_receive_args = {.m_windowstate = WindowState_Connected, .m_main_res = main_res};
    */
    double time = 0;
    window_draw_func draw = DRAW_NONE;

    while (!WindowShouldClose()) {
        switch (windowstate) {
        case WindowState_AttemptConnection: {
            draw = draw_try_connect;

            if (!client_connect_thread.m_active) {
                client_connect_args.m_windowstate = windowstate;
                client_connect_args.m_main_res = main_res;
                int thrd_create_res = thrd_create(&client_connect_thread.m_thread, client_connect, &client_connect_args);

                if (thrd_create_res != thrd_success) {
                    ERR_TO_WIN(1, "Failed to create a thread for accepting a client.");
                    break;
                } else {
                    client_connect_thread.m_active = true;
                    LOG("main(): Thread for accepting client successfully created.");
                }
            } else if (client_connect_args.m_windowstate != windowstate) {
                windowstate = client_connect_args.m_windowstate;
                main_res = client_connect_args.m_main_res;
                time = GetTime();
                client_connect_thread.m_active = false;
                LOG("main(): Switching to WindowState %d after Thread client_connect() has finished.", windowstate);
            }
        } break;

        case WindowState_Connected: {
            draw = draw_connected;

            // TODO: Setup receive thread

            if (GetTime() - time > CONNECTED_WAIT_SECS) {
                LOG("main(): Switching to WindowState_Minimap %f second(s) after connection.", CONNECTED_WAIT_SECS);
                windowstate = WindowState_Minimap;
            }
        } break;

        case WindowState_Minimap: {
            draw = draw_minimap;

            // TODO: Setup receive thread
        } break;

        case WindowState_Received: {
            draw = draw_minimap;
            // TODO: Send received string to window.c, where it should update static variables accordingly for draw_minimap to work
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