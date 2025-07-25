#ifndef MESSAGE_H
#define MESSAGE_H

#include "types.h"

/*
 * Messages exchanged with the Lua script will always be only be u8 arrays.
 * == HEADER ==
 * msg[0]: The message type (according to enum Msg)
 * msg[1..3]: The size of the following data in bytes (little endian)
 * == DATA ==
 * msg[4..16777218]: Data that is processed according to the message type
 */

#define MSG_TYPE(msg) (msg[0])
#define MSG_ARGS_SIZE(msg) (msg[1] | (msg[2] << 0x8) | (msg[3] << 0x10))

#define IS_HEADER_EMPTY(msg) (msg[0] == 0 && msg[1] == 0 && msg[2] == 0 && msg[3] == 0)

enum Msg {
    MsgIdentify = 0,
    MsgIngame = 1,
    MsgChangeRoom = 2,
    MsgCoordinates = 3,
    MsgRoomDimensions = 4,
};

// The following functions all take pointers to the message and an empty struct and initialise the latter for further use.

// This struct is valid as long as the message has not yet been freed.
struct MsgIdentify {
    u8* m_id;
};
void msg_identify(struct MsgIdentify*, u8*);

struct MsgIngame {
    bool m_ingame;
};
void msg_ingame(struct MsgIngame*, u8*);

struct MsgChangeRoom{
    u16 m_room;
};
void msg_change_room(struct MsgChangeRoom*, u8*);

struct MsgCoordinates {
    s32 m_x;
    s32 m_y;
};
void msg_coordinates(struct MsgCoordinates*, u8*);

struct MsgRoomDimensions {
    u16 m_width;
    u16 m_height;
};
void msg_room_dimensions(struct MsgRoomDimensions*, u8*);

#endif  // MESSAGE_H
