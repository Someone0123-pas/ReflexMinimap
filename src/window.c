#include "window.h"

#include <raylib.h>
#include <stdio.h>

#include "error.h"
#include "log.h"
#include "messageformat.h"

#define NONUM 0
static struct {
    bool m_ingame;
    int m_room;
    int m_x;
    int m_y;
} s_kirbystate = {false, NONUM, NONUM, NONUM};

void kirbystate_init() {
    s_kirbystate.m_ingame = false;
    s_kirbystate.m_room = NONUM;
    s_kirbystate.m_x = NONUM;
    s_kirbystate.m_y = NONUM;
}


[[nodiscard]] result_t kirbystate_update(u8* message) {
    switch (message[0]) {
    case MsgIdentify: {
        // TODO
    } break;
    case MsgIngame: {
        struct MsgIngame windowdata;
        msg_ingame(&windowdata, message);
        s_kirbystate.m_ingame = windowdata.m_ingame;
    } break;
    case MsgRoom: {
        struct MsgRoom windowdata;
        msg_room(&windowdata, message);
        s_kirbystate.m_room = windowdata.m_room;
    } break;
    case MsgCoordinates: {
        struct MsgCoordinates windowdata;
        msg_coordinates(&windowdata, message);
        s_kirbystate.m_x = windowdata.m_x;
        s_kirbystate.m_y = windowdata.m_y;
    } break;
    default: {
        WARN("window_update(): The received message came with unknown message type %d.", message[0]);
        return KIRBYSTATE_UPDATE_UNKNOWN_MSGTYPE;
    }
    }
    return OK;
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

window_draw draw_minimap(Font* font) {
    if (s_kirbystate.m_ingame) {
        if (s_kirbystate.m_room != NONUM) {
            char text[0x20] = {};
            snprintf(text, sizeof(text), "Room ID: %d", s_kirbystate.m_room);
            DrawTextEx(*font, text, (Vector2){20, 20}, 18.0f, 0.0f, BLACK);
        }

        if (s_kirbystate.m_x != NONUM) {
            char text_x[0x20] = {};
            snprintf(text_x, sizeof(text_x), "X: %d", s_kirbystate.m_x >> 0xc);
            float fontsize_x = 18.0f;
            float fontspacing_x = 0.0f;
            Vector2 textsize_x = MeasureTextEx(*font, text_x, fontsize_x, fontspacing_x);
            Vector2 textpos_x = {((float)GetScreenWidth() - textsize_x.x) / 2, 20};
            DrawTextEx(*font, text_x, textpos_x, fontsize_x, fontspacing_x, BLACK);

            char text_y[0x20] = {};
            snprintf(text_y, sizeof(text_y), "Y: %d", s_kirbystate.m_y >> 0xc);
            float fontsize_y = 18.0f;
            float fontspacing_y = 0.0f;
            Vector2 textsize_y = MeasureTextEx(*font, text_y, fontsize_y, fontspacing_y);
            Vector2 textpos_y = {(float)GetScreenWidth() - textsize_y.x - 20, 20};
            DrawTextEx(*font, text_y, textpos_y, fontsize_y, fontspacing_y, BLACK);
        }
    } else {
        const char text[] = "MENU";
        float fontsize = 30.0f;
        float fontspacing = 2.0f;
        Vector2 textsize = MeasureTextEx(*font, text, fontsize, fontspacing);
        Vector2 textpos = {((float)GetScreenWidth() - textsize.x) / 2, ((float)GetScreenHeight() - textsize.y) / 2};
        DrawTextEx(*font, text, textpos, fontsize, fontspacing, BLACK);
    }
}

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
