#include "messageformat.h"

#include <raylib.h>
#include <stdlib.h>

#include "log.h"
#include "types.h"
#include "window.h"

void msg_identify(struct MsgIdentify* res, u8* msg) {
    res->m_id = msg + 2;
    LOG("msg_identify(): Received identifying string.");
    // TODO: Return bool valid_identifier
}

void msg_ingame(struct MsgIngame* res, u8* msg) {
    res->m_ingame = (bool)msg[4];
    LOG("msg_ingame(): m_ingame was set to %s.", (res->m_ingame ? "TRUE" : "FALSE"));
}

void msg_change_room(struct MsgChangeRoom* res, u8* msg) {
    res->m_room = msg[4] | (msg[5] << 0x8);
    LOG("msg_room(): Received Room-ID %d.", res->m_room);
}

void msg_coordinates(struct MsgCoordinates* res, u8* msg) {
    res->m_x = msg[4] | (msg[5] << 0x8) | (msg[6] << 0x10) | (msg[7] << 0x18);
    res->m_y = msg[8] | (msg[9] << 0x8) | (msg[0xa] << 0x10) | (msg[0xb] << 0x18);
    res->m_xspeed = (s16)(msg[0xc] | (msg[0xd] << 0x8));
    res->m_yspeed = (s16)(msg[0xe] | (msg[0xf] << 0x8));
    LOG("msg_coordinates(): Received X = %d, Y = %d, XSpeed = %d and YSpeed = %d", res->m_x, res->m_y, res->m_xspeed,
        res->m_yspeed);
}

void msg_room_dimensions(struct MsgRoomDimensions* res, u8* msg) {
    res->m_width = msg[4] | (msg[5] << 0x8);
    res->m_height = msg[6] | (msg[7] << 0x8);
    LOG("msg_room_dimensions(): Received Width = %d and Height = %d.", res->m_width, res->m_height);
}

static void solidity_colors(Color* res, u8 solidity) {
    switch (solidity) {
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c: {
        // Slopes
        *res = BLACK;
    } break;
    case 0x50:
    case 0xf2:
    case 0x0d: {
        // Solids
        *res = BLACK;
    } break;
    case 0x0e: {
        // Platform (fallthrough)
        *res = GREEN;
    } break;
    case 0x0f: {
        // Platform
        *res = DARKGREEN;
    } break;
    case 0x48: {
        // Water
        *res = BLUE;
    } break;
    case 0xfd:
    case 0xff: {
        // Door
        *res = YELLOW;
    } break;
    default: {
        *res = BLANK;
    } break;
    }
}

void msg_solidity(struct MsgSolidity* res, u8* msg, u16 width, u16 height) {
    res->m_solidity_size = msg[1] | (msg[2] << 0x8) | (msg[3] << 0x10);
    LOG("msg_solidity(): Received solidity with width %d", res->m_solidity_size);

    Color* imagedata = (Color*)calloc(res->m_solidity_size, sizeof(Color));
    for (u32 tile = 0; tile < res->m_solidity_size; tile++) {
        Color color = {};
        solidity_colors(&color, msg[tile + 4]);
        imagedata[tile] = color;
    }

    Image img = {
        .data = imagedata,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    ImageResizeNN(&img, width * 2 * COOR_SCALING, height * 2 * COOR_SCALING);

    *res->m_solidity_texture = LoadTextureFromImage(img);
    UnloadImage(img);
}
