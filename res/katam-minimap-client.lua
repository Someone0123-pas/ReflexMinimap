--[[
From messageformat.h:
/*
 * Messages exchanged with the Lua script will always be only be u8 arrays.
 * == HEADER ==
 * msg[0]: The message type (according to enum Msg)
 * msg[1]: The size of the following data in bytes (0x00-0xff)
 * == DATA ==
 * msg[2..257]: Data that is processed according to the message type
 */
]]

MsgSuccess = 0
MsgIdentify = 1
MsgIngame = 2
MsgRoom = 3
MsgCoordinates = 4

port = 2346
serversocket = nil

function serversocket_err(error)
    console:error("Closing connection because of error: " .. error)
    serversocket_close()
end

function serversocket_recv()
end

function serversocket_close()
    if not serversocket then return end
    console:log("Closing connection.")
    serversocket:close()
    serversocket = nil
end


function client_frameadvance()
end

function client_main()
    callbacks:add("frame", client_frameadvance)
    serversocket:send(string.char(MsgIdentify, 4) .. "mGBA")
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
    end
end


client_init()
serversocket_close()