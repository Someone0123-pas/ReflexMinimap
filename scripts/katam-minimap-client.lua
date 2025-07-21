port = 2346
serversocket = nil

function serversocket_err()
end

function serversocket_recv()
end

function serversocket_close()
    if not serversocket then return end
    console:log("Closing connection.")
    serversocket:close()
    serversocket = nil
end

function client_init()
    console:log("=== KATAM-Minimap by Someone0123-pas v0.0.0 ===")
    console:log(string.format("Trying to connect to 127.0.0.1:%d ...", port))

    serversocket = socket.tcp()
    serversocket:add("error", serversocket_err)
    serversocket:add("received", serversocket_recv)

    if serversocket:connect("127.0.0.1", port) then
        console:log("Successfully connected!")
    else
        console:error("Could not connect to server. Has the program already been started?")
        serversocket_close()
    end
end

client_init()
serversocket_close()
-- callbacks:add("frame", client_frameadvance)