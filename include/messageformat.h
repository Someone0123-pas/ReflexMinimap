#ifndef MESSAGE_H
#define MESSAGE_H

#include "types.h"

/*
 * Messages exchanged with the Lua script will always be only be u8 arrays.
 * == HEADER ==
 * msg[0]: The message type (according to enum Msg)
 * msg[1]: The size of the following data in bytes (0x00-0xff)
 * == DATA ==
 * msg[2..257]: Data that is processed according to the message type
 */

#define MSG_TYPE(msg) (msg[0])
#define MSG_ARGS_SIZE(msg) (msg[1])

enum Msg {
    MsgIdentify = 0,
    MsgIngame = 1,
    MsgRoom = 2,
    MsgCoordinates = 3,
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

struct MsgRoom {
    u16 m_room;
};
void msg_room(struct MsgRoom*, u8*);

struct MsgCoordinates {
    s32 m_x;
    s32 m_y;
};
void msg_coordinates(struct MsgCoordinates*, u8*);

#endif  // MESSAGE_H
