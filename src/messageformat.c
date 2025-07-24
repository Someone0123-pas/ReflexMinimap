#include "messageformat.h"

void msg_identify(struct MsgIdentify* res, u8* msg) { res->m_id = msg + 2; }

void msg_ingame(struct MsgIngame* res, u8* msg) { res->m_ingame = (bool)msg[2]; }

void msg_room(struct MsgRoom* res, u8* msg) { res->m_room = msg[2] | (msg[3] << 0x8); }

void msg_coordinates(struct MsgCoordinates* res, u8* msg) {
    res->m_x = msg[2] | (msg[3] << 0x8) | (msg[4] << 0x10) | (msg[5] << 0x18);
    res->m_y = msg[6] | (msg[7] << 0x8) | (msg[8] << 0x10) | (msg[9] << 0x18);
}
