#include "window.h"

#include <raylib.h>
#include <stdio.h>

#include "error.h"
#include "log.h"
#include "messageformat.h"
#include "types.h"

int g_fps = 240;

constexpr u8 ROOM_CHANGE_WAIT_FRAMES = 30;

constexpr u8 COOR_INTERPOL_TIMES = 1;
constexpr int COOR_INTERPOL_DENOMINATOR = 0x8;

#define NONUM 0
static struct {
    bool m_ingame;
    u8 m_room_change_timer;
    u8 m_coor_do_interpol;
    u16 m_room;
    s32 m_x;
    s32 m_y;
    s16 m_xspeed;
    s16 m_yspeed;
    u16 m_width;
    u16 m_height;
    u32 m_solidity_size;
    Texture2D m_solidity_texture;
} s_kirbystate = {};

void kirbystate_init() {
    s_kirbystate.m_ingame = false;
    s_kirbystate.m_room_change_timer = 0;
    s_kirbystate.m_coor_do_interpol = 0;
    s_kirbystate.m_room = NONUM;
    s_kirbystate.m_x = NONUM;
    s_kirbystate.m_y = NONUM;
    s_kirbystate.m_xspeed = NONUM;
    s_kirbystate.m_yspeed = NONUM;
    s_kirbystate.m_width = NONUM;
    s_kirbystate.m_height = NONUM;
    UnloadTexture(s_kirbystate.m_solidity_texture);
    s_kirbystate.m_solidity_texture.id = 0;
}

[[nodiscard]] result_t kirbystate_update(u8* message) {
    switch (message[0]) {
    case MsgIdentify: {
        // TODO
    } break;
    case MsgIngame: {
        struct MsgIngame windowdata = {};
        msg_ingame(&windowdata, message);
        s_kirbystate.m_ingame = windowdata.m_ingame;
    } break;
    case MsgChangeRoom: {
        struct MsgChangeRoom windowdata = {};
        msg_change_room(&windowdata, message);
        s_kirbystate.m_room = windowdata.m_room;
        s_kirbystate.m_room_change_timer = (ROOM_CHANGE_WAIT_FRAMES * g_fps) / 60;
    } break;
    case MsgCoordinates: {
        struct MsgCoordinates windowdata = {};
        msg_coordinates(&windowdata, message);
        s_kirbystate.m_x = windowdata.m_x;
        s_kirbystate.m_y = windowdata.m_y;
        s_kirbystate.m_xspeed = windowdata.m_xspeed;
        s_kirbystate.m_yspeed = windowdata.m_yspeed;
        s_kirbystate.m_coor_do_interpol = (COOR_INTERPOL_TIMES * g_fps) / 60;
    } break;
    case MsgRoomDimensions: {
        struct MsgRoomDimensions windowdata = {};
        msg_room_dimensions(&windowdata, message);
        s_kirbystate.m_width = windowdata.m_width;
        s_kirbystate.m_height = windowdata.m_height;
    } break;
    case MsgSolidity: {
        if (s_kirbystate.m_width == 0 || s_kirbystate.m_height == 0) {
            WARN("kirbystate_update(): Solidity received without prior room dimensions. Skipping solidity.");
            break;
        }
        UnloadTexture(s_kirbystate.m_solidity_texture);
        s_kirbystate.m_solidity_texture.id = 0;

        struct MsgSolidity windowdata = {.m_solidity_texture = &s_kirbystate.m_solidity_texture};
        msg_solidity(&windowdata, message, s_kirbystate.m_width / 2, s_kirbystate.m_height / 2);
        s_kirbystate.m_solidity_size = windowdata.m_solidity_size;
    } break;
    default: {
        WARN("kirbystate_update(): The received message came with unknown message type %d.", message[0]);
        return KIRBYSTATE_UPDATE_UNKNOWN_MSGTYPE;
    }
    }
    return OK;
}

window_draw draw_try_connect(Font* font, Camera2D* camera) {
    DrawTextEx(*font, "KATAM-Minimap\nby Someone0123-pas\nv0.0.0", (Vector2){20, 20}, 25, 0.5f, BLACK);
    DrawTextEx(*font, "Listening to port 2346 ...", (Vector2){20, 120}, 20, 0.0f, DARKPURPLE);
}

window_draw draw_connected(Font* font, Camera2D* camera) {
    const char text[] = "Connected!";
    float fontsize = 30.0f;
    float fontspacing = 2.0f;
    Vector2 textsize = MeasureTextEx(*font, text, fontsize, fontspacing);
    Vector2 textpos = {((float)GetScreenWidth() - textsize.x) / 2.0f, ((float)GetScreenHeight() - textsize.y) / 2.0f};
    DrawTextEx(*font, text, textpos, fontsize, fontspacing, GREEN);
}

window_draw draw_minimap(Font* font, Camera2D* camera) {
    if (s_kirbystate.m_ingame) {
        if (s_kirbystate.m_x != NONUM) {
            camera->target =
                (Vector2){(float)((s_kirbystate.m_x * COOR_SCALING) >> 0xb), (float)((s_kirbystate.m_y * COOR_SCALING) >> 0xb)};
        }

        if (s_kirbystate.m_coor_do_interpol) {
            s_kirbystate.m_x += s_kirbystate.m_xspeed / COOR_INTERPOL_DENOMINATOR;
            s_kirbystate.m_y += s_kirbystate.m_yspeed / COOR_INTERPOL_DENOMINATOR;
        }

        // DYNAMIC CAMERA

        BeginMode2D(*camera);

        if (!s_kirbystate.m_room_change_timer && s_kirbystate.m_width != NONUM && s_kirbystate.m_x != NONUM &&
            s_kirbystate.m_solidity_texture.id != 0) {
            float line_thick = (float)PIXELSIZE/2.0f;
            float width_room = (float)(s_kirbystate.m_width) * COOR_SCALING + line_thick * 2;
            float height_room = (float)(s_kirbystate.m_height) * COOR_SCALING + line_thick * 2;
            Rectangle room_borders = {-line_thick, -line_thick, width_room, height_room};
            DrawRectangleLinesEx(room_borders, line_thick, BLACK);

            DrawTexture(s_kirbystate.m_solidity_texture, 0, 0, WHITE);

            DrawRectangle(((s_kirbystate.m_x * COOR_SCALING) >> 0xb) - PIXELSIZE / 2,
                          ((s_kirbystate.m_y * COOR_SCALING) >> 0xb) - PIXELSIZE / 2, PIXELSIZE, PIXELSIZE, RED);
        }

        EndMode2D();

        // STATIC HEADER

        if (s_kirbystate.m_room != NONUM) {
            DrawTextEx(*font, TextFormat("Room ID: %d", s_kirbystate.m_room), (Vector2){20, 20}, 18.0f, 0.0f, BLACK);
        }

        if (s_kirbystate.m_x != NONUM) {
            const char* text_x = TextFormat("X: %d", s_kirbystate.m_x >> 0xc);
            float fontsize_x = 18.0f;
            float fontspacing_x = 0.0f;
            Vector2 textsize_x = MeasureTextEx(*font, text_x, fontsize_x, fontspacing_x);
            Vector2 textpos_x = {((float)GetScreenWidth() - textsize_x.x) / 2.0f, 20};
            DrawTextEx(*font, text_x, textpos_x, fontsize_x, fontspacing_x, BLACK);

            const char* text_y = TextFormat("Y: %d", s_kirbystate.m_y >> 0xc);
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

    if (s_kirbystate.m_room_change_timer > 0) {
        s_kirbystate.m_room_change_timer--;
    }
    if (s_kirbystate.m_coor_do_interpol > 0) {
        s_kirbystate.m_coor_do_interpol--;
    }
}

window_draw draw_error(Font* font, Camera2D* camera) {
    DrawText("ERROR!", 20, 20, 30, RED);
    // TODO: Use TextFormat()
    char errorstring[0x200] = {0};
    if (WAS_ERRNO_USED) {
        snprintf(errorstring, sizeof(errorstring), "%s:\n%s", g_errstr_custom, g_errstr_errno);
    } else {
        snprintf(errorstring, sizeof(errorstring), "%s", g_errstr_custom);
    }
    DrawTextEx(*font, errorstring, (Vector2){20, 20}, 20, 1.0f, BLACK);
}
