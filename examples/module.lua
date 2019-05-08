function preInit ()
  print('[LOG] Module v1.0.0 loaded.')
  print('[LOG] Server hash is:', getServerHash())
end

function postInit()
  print('[LOG] Server is ready.')
end

function peerConnect (peer)
  print('[LOG] Connected:', peer)
  gp = packetEnd(appendString(appendString(createPacket(), 'OnConsoleMessage'), 'Welcome to the server!'))
  sendGamePacket(peer, gp)
  return false
end

function peerReceive (peer, data)
  print('[LOG] Received:', peer)
  print(data)
  return false
end

function peerDisconnect (peer)
  print('[LOG] Disconnected:', peer)
  return false
end

function serverShutdown ()
  print('[LOG] Closing server...')
end