extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

lua_State* L;

static void disconnect(lua_State *L) {
  int n = lua_gettop(L);
  ENetPeer* peer = (ENetPeer*) lua_touserdata(L, 1);

  if (peer->state != ENET_PEER_STATE_CONNECTED) {
    lua_pushstring(L, "Peer is not connected!");
    lua_error(L);
  }

  enet_peer_disconnect_later(peer, 0);
}

void initLua () {
  L = luaL_newstate();
  luaL_openlibs(L);
  if (luaL_dofile(L, "module.lua")) {
    cout << "Module not loaded." << endl;
  }
}

void preInitHandler (lua_State *L) {
  lua_getglobal(L, "preInit");
  lua_call(L, 0, 0);
  lua_settop(L, 0);
}

void postInitHandler (lua_State *L) {
  lua_getglobal(L, "postInit");
  lua_call(L, 0, 0);
  lua_settop(L, 0);
}

bool peerConnectHandler (lua_State* L, ENetPeer* peer) {
  lua_getglobal(L, "peerConnect");
  lua_pushlightuserdata(L, (void*)peer);
  lua_call(L, 1, 1);

  bool result = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return result;
}

bool peerReceiveHandler (lua_State* L, ENetPeer* peer, ENetPacket* packet) {
  lua_getglobal(L, "peerReceive");
  lua_pushlightuserdata(L, (void*)peer);
  lua_pushlstring (L, (char*)packet->data, packet->dataLength);
  lua_call(L, 2, 1);

  bool result = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return result;
}

void peerDisconnectHandler (lua_State* L, ENetPeer* peer) {
  lua_getglobal(L, "peerDisconnect");
  lua_pushlightuserdata(L, (void*)peer);
  lua_call(L, 1, 0);
}

void serverShutdownHandler (lua_State* L) {
  lua_getglobal(L, "serverShutdown");
  lua_call(L, 0, 0);
  lua_close(L);
}
