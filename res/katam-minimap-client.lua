-- CONFIGURATION --

-- Send update messages to the server all X messages.
-- Recommendation: Increase, if the emulator performance degrades when starting this script.
-- Default Value: 5
WAITING_FRAMES_PER_CYCLE = 2

-- How often the solidity map is refreshed (unlockable doors, broken rocks, etc.) in respect to the WAITING_FRAMES_PER_CYCLE
-- Recommendation: It makes sense for this not to be the same as above (i.e. 1)
-- Default Value: 4
WAITING_TIMES_SOLIDITY = 4

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
MsgSolidity = 5

AdrRoomidKirby0 = 0x02020f40
AdrXKirby0 = 0x02020f20
AdrYKirby0 = 0x02020f24
AdrRoomProps = 0x089331ac
AdrForegroundTilemaps = 0x08d64520
AdrSolidityMaps = 0x08d63330
AdrSolidityMapDecompressed = 0x02024ed0
AdrNumSolidityMap = 0x02023b8e

-- It makes sense not to send the solidity map as often as coordinates

NONUM = 0

port = 2346
serversocket = nil
frame_timer = 0

ingame = false
roomid = NONUM
x = NONUM
y = NONUM
solidity_timer = 0

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
    send_solidity()
end

function send_coordinates()
    x = emu:read32(AdrXKirby0)
    y = emu:read32(AdrYKirby0)
    serversocket_send(MsgCoordinates, emu:readRange(AdrXKirby0, 8) .. emu:readRange(AdrXKirby0 + 0x10, 4))
end

function send_roomdim()
    roomid = emu:read16(AdrRoomidKirby0)
    local map_data_index = emu:read16(AdrRoomProps + 0x28 * roomid + 0x18)
    local adr_foreground_tilemap = emu:read32(AdrForegroundTilemaps + 0x4 * map_data_index)
    serversocket_send(MsgRoomDimensions, emu:readRange(adr_foreground_tilemap, 4))
end

function send_solidity()
    roomid = emu:read16(AdrRoomidKirby0)
    local num_solidity = emu:read8(AdrNumSolidityMap)
    local solidity_map_index = emu:read16(AdrRoomProps + 0x28 * roomid + 0x1a)
    local adr_solidity_map = emu:read32(AdrSolidityMaps + 0x4 * solidity_map_index)
    local size_solidity = bit32.extract(emu:read32(emu:read32(adr_solidity_map)), 0x8, 0x18)
    serversocket_send(MsgSolidity, emu:readRange(AdrSolidityMapDecompressed + 0x79e * num_solidity, size_solidity))
end

function client_frameadvance()

    if frame_timer ~= 0 then
        frame_timer = frame_timer - 1
        return
    end
    frame_timer = WAITING_FRAMES_PER_CYCLE

    if solidity_timer ~= 0 then
        solidity_timer = solidity_timer-1
    end

    if ingame then
        if roomid ~= emu:read16(AdrRoomidKirby0) then
            send_change_room()
        end
        if x ~= emu:read32(AdrXKirby0) or y ~= emu:read32(AdrYKirby0) then
            send_coordinates()
            if solidity_timer == 0 then
                solidity_timer = WAITING_TIMES_SOLIDITY
                send_solidity()
            end
        end
        if roomid == 0 then
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