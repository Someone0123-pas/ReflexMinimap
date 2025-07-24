#include "window.h"

#include <raylib.h>
#include <stdio.h>

#include "error.h"
#include "messageformat.h"

result_t window_update(u8* message) {
    // TODO: Update static variables for draw_minimap() to use
}

window_draw draw_try_connect(Font* font) {
    DrawTextEx(*font, "KATAM-Minimap\nby Someone0123-pas\nv0.0.0", (Vector2){20, 20}, 25, 0.5f, BLACK);
    DrawTextEx(*font, "Listening to port 2346 ...", (Vector2){20, 120}, 20, 0.0f, DARKPURPLE);
}

window_draw draw_connected(Font* font) {
    const char text[] = "Connected!";
    float fontsize = 30.0f;
    float fontspacing = 2.0f;
    Vector2 textsize = MeasureTextEx(*font, text, fontsize, fontspacing);
    Vector2 textpos = {((float)GetScreenWidth() - textsize.x) / 2, ((float)GetScreenHeight() - textsize.y) / 2};
    DrawTextEx(*font, text, textpos, fontsize, fontspacing, GREEN);
}

window_draw draw_minimap(Font* font) { DrawTextEx(*font, "Minimap", (Vector2){20, 20}, 25, 0.5f, BLACK); }

window_draw draw_error(Font* font) {
    DrawText("ERROR!", 20, 20, 30, RED);
    char errorstring[0x200] = {0};
    if (WAS_ERRNO_USED) {
        snprintf(errorstring, sizeof(errorstring), "%s:\n%s", g_errstr_custom, g_errstr_errno);
    } else {
        snprintf(errorstring, sizeof(errorstring), "%s", g_errstr_custom);
    }
    DrawTextEx(*font, errorstring, (Vector2){20, 20}, 20, 1.0f, BLACK);
}
