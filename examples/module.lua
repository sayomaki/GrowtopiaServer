function preInit ()
  print('[LOG] Module v1.0.0 loaded.')
end

function postInit()
  print('[LOG] Server is ready.')
end

function peerConnect (peer)
  print('[LOG] Connected:', peer)
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