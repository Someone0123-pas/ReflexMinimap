-- CONFIGURATION --

-- Send update messages to the server all X messages.
-- Recommendation: Increase, if the emulator performance degrades when starting this script.
-- Default Value: 4
WAITING_FRAMES_PER_CYCLE = 3

-- CONFIGURATION END --

--[[
From messageformat.h:
/*
 * Messages exchanged with the Lua script will always only be u8 arrays.
 * == HEADER ==
 * msg[0]: The message type (according to enum Msg)
 * msg[1..3]: The size of the following data in bytes (little endian)
 * == DATA ==
 * msg[4..16777218]: Data that is processed according to the message type
 */
]]

MsgIdentify = 0
MsgIngame = 1
MsgChangeRoom = 2
MsgCoordinates = 3
MsgRoomDimensions = 4

AdrRoomidKirby0 = 0x02020f40
AdrXKirby0 = 0x02020f20
AdrYKirby0 = 0x02020f24
AdrRoomProps = 0x089331ac
AdrForegroundTilemaps = 0x08d64520

port = 2346
serversocket = nil
timer = 0

ingame = false
roomid = nil
x = nil
y = nil
NONUM = 0

function serversocket_err(error)
    console:error("Closing connection because of error: " .. error)
    serversocket_close()
end

function serversocket_recv()
-- TODO
end

function serversocket_close()
    if not serversocket then return end
    console:log("Closing connection.")
    serversocket:close()
    serversocket = nil
    console:log("=== Ending Script ===")
end

function serversocket_send(type, msgstring)
    if msgstring:len() > 0xffffff then
        console:error("Message \"" .. msgstring .. "\" is too long!")
        return
    end
    local msgstringlen = msgstring:len()
    serversocket:send(string.char(type,
        bit32.extract(msgstringlen, 0x00, 8),
        bit32.extract(msgstringlen, 0x08, 8),
        bit32.extract(msgstringlen, 0x10, 8)) .. msgstring)
end

function send_identify()
    serversocket_send(MsgIdentify, "mGBA")
end

function send_ingame(is_ingame)
    ingame = is_ingame
    if ingame then
        serversocket_send(MsgIngame, string.char(1))
    else
        serversocket_send(MsgIngame, string.char(0))
    end
end

-- TODO: Take index of Kirby instance,
-- now only Kirby 0 is used

function send_change_room()
    roomid = emu:read16(AdrRoomidKirby0)
    serversocket_send(MsgChangeRoom, emu:readRange(AdrRoomidKirby0, 2))
    send_roomdim()
    -- TODO: Send Solidity Map
end

function send_coordinates()
    x = emu:read32(AdrXKirby0)
    y = emu:read32(AdrYKirby0)
    serversocket_send(MsgCoordinates, emu:readRange(AdrXKirby0, 8))
end

function send_roomdim()
    roomid = emu:read16(AdrRoomidKirby0)
    local map_data_index = emu:read16(AdrRoomProps + 0x28 * roomid + 0x18)
    local adr_foreground_tilemap = emu:read32(AdrForegroundTilemaps + 0x4 * map_data_index)
    serversocket_send(MsgRoomDimensions, emu:readRange(adr_foreground_tilemap, 4))
end

function client_frameadvance()
    if timer ~= 0 then
        timer = timer - 1
        return
    end
    timer = WAITING_FRAMES_PER_CYCLE

    if ingame then
        if roomid ~= emu:read16(AdrRoomidKirby0) then
            send_change_room()
        end
        if x ~= emu:read32(AdrXKirby0) or y ~= emu:read32(AdrYKirby0) then
            send_coordinates()
        end
        if roomid == NONUM then
            send_ingame(false)
        end
    else
        if emu:read16(AdrRoomidKirby0) ~= NONUM then
            send_ingame(true)
        end
    end
end

function client_main()
    callbacks:add("frame", client_frameadvance)
    send_identify()
end

function client_init()
    console:log("=== KATAM-Minimap by Someone0123-pas v0.0.0 ===")
    console:log(string.format("Trying to connect to 127.0.0.1:%d ...", port))

    serversocket = socket.tcp()
    serversocket:add("error", serversocket_err)
    serversocket:add("received", serversocket_recv)

    if serversocket:connect("127.0.0.1", port) then
        console:log("Successfully connected!")
        client_main()
    else
        console:error("Could not connect to server. Please start the minimap application first, and then rerun the script.")
        console:log("=== Ending Script ===")
    end
end

callbacks:add("crashed", serversocket_close)
callbacks:add("shutdown", serversocket_close)
callbacks:add("stop", serversocket_close)
client_init()