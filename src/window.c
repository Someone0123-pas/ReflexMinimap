#include "window.h"

#include <raylib.h>
#include <stdio.h>

#include "error.h"
#include "messageformat.h"

window_draw draw_try_connect() { DrawText("=== KATAM-Minimap by Someone0123-pas v0.0.0 ===\nListening to port 2346 ...", 20, 20, 20, BLACK); }

window_draw draw_connected() { DrawText("Connected!", 20, 20, 30, GREEN); }

window_draw draw_minimap() { DrawText("Minimap", 20, 20, 20, BLACK); }

window_draw draw_error() {
    DrawText("ERROR!", 20, 20, 30, RED);
    char errorstring[0x200] = {0};
    if (WAS_ERRNO_USED) {
        snprintf(errorstring, sizeof(errorstring), "%s:\n%s", g_errstr_custom, g_errstr_errno);
    } else {
        snprintf(errorstring, sizeof(errorstring), "%s", g_errstr_custom);
    }
    DrawText(errorstring, 20, 80, 20, BLACK);
}

result_t update_window(u8* message) {
    // TODO
}
