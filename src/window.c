#include "window.h"

#include <raylib.h>
#include <stdio.h>

#include "error.h"

window_draw draw_try_connect() { DrawText("=== KATAM-Minimap by Someone0123-pas v0.0.0 ===\nListening to port 2346 ...", 50, 50, 20, BLACK); }

window_draw draw_connected() { DrawText("Connected!", 50, 50, 30, GREEN); }

window_draw draw_error() {
    DrawText("ERROR!", 50, 50, 30, RED);
    char errorstring[0x200] = {0};
    if (WAS_ERRNO_USED) {
        snprintf(errorstring, sizeof(errorstring), "%s:\n%s", g_errstr_custom, g_errstr_errno);
    } else {
        snprintf(errorstring, sizeof(errorstring), "%s", g_errstr_custom);
    }
    DrawText(errorstring, 50, 100, 20, BLACK);
}
