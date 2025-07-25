#include "messageformat.h"

#include "log.h"

void msg_identify(struct MsgIdentify* res, u8* msg) {
    res->m_id = msg + 2;
    LOG("msg_identify(): Received identifying string.");
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
