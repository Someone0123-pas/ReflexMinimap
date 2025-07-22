#include "window.h"

#include <raylib.h>

#include "error.h"

window_draw draw_try_connect() { DrawText("Hello world!", 100, 100, 20, BLACK); }

window_draw draw_connected() {
    // TODO
}

window_draw draw_error() {
    // TODO: Print g_errorstr and errno
}
