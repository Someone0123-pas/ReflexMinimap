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