#ifndef MESSAGE_H
#define MESSAGE_H

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
    MsgSuccess,
    MsgIdentify,
    MsgIngame,
    MsgRoom,
    MsgCoordinates,
};

// TODO: Functions that take the u8* message and an adequate struct* and copy information from one to the other

#endif  // MESSAGE_H
