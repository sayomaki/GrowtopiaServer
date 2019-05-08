#include "functions.h"

extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
}

lua_State* L;

static int disconnect(lua_State *L) {
  int num = lua_gettop(L);
  ENetPeer* peer = (ENetPeer*) lua_touserdata(L, 1);

  if (peer->state != ENET_PEER_STATE_CONNECTED) {
    lua_pushstring(L, "Peer is not connected!");
    lua_error(L);
  }

  enet_peer_disconnect_later(peer, 0);
  return 0;
}

static int GPcreatePacket(lua_State *L) {
  unsigned char* data = new unsigned char[61];
	string asdf = "0400000001000000FFFFFFFF00000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	for (int i = 0; i < asdf.length(); i += 2)
	{
		char x = ch2n(asdf[i]);
		x = x << 4;
		x += ch2n(asdf[i + 1]);
		memcpy(data + (i / 2), &x, 1);
		if (asdf.length() > 61 * 2) throw 0;
	}
  GamePacket* packet = new GamePacket();
	packet->data = data;
	packet->len = 61;
	packet->indexes = 0;
	
  lua_pushlightuserdata(L, (void*)packet);
  return 1;
}

static int GPappendStr (lua_State *L) {
  int num = lua_gettop(L);
  GamePacket* p = (GamePacket*) lua_touserdata(L, 1);
  string str = lua_tostring(L, 2);

  unsigned char* n = new unsigned char[p->len + 2 + str.length() + 4];
	memcpy(n, p->data, p->len);
	delete p->data;
	p->data = n;
	n[p->len] = p->indexes;
	n[p->len + 1] = 2;
	int sLen = str.length();
	memcpy(n+p->len+2, &sLen, 4);
	memcpy(n + p->len + 6, str.c_str(), sLen);
	p->len = p->len + 2 + str.length() + 4;
	p->indexes++;

  lua_pushlightuserdata(L, (void*)p);
  return 1;
}

static int GPpacketEnd (lua_State *L) {
  int num = lua_gettop(L);
  GamePacket* p = (GamePacket*) lua_touserdata(L, 1);

  unsigned char* n = new unsigned char[p->len + 1];
	memcpy(n, p->data, p->len);
	delete p->data;
	p->data = n;
	char zero = 0;
	memcpy(p->data+p->len, &zero, 1);
	p->len += 1;
	*(int*)(p->data + 56) = p->indexes;
	*(BYTE*)(p->data + 60) = p->indexes;

  lua_pushlightuserdata(L, (void*)p);
  return 1;
}

static int peerSendGP (lua_State *L) {
  int num = lua_gettop(L);
  ENetPeer* peer = (ENetPeer*) lua_touserdata(L, 1);
  GamePacket* p = (GamePacket*) lua_touserdata(L, 2);

  ENetPacket * packet = enet_packet_create(p->data,
    p->len,
    ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, packet);

  return 0;
}

static int EXsendWorldOffers (lua_State *L) {
  int num = lua_gettop(L);
  ENetPeer* peer = (ENetPeer*) lua_touserdata(L, 1);
  
  if (!((PlayerInfo*)(peer->data))->isIn) return 0;
  vector<WorldInfo> worlds = worldDB.getRandomWorlds();
  string worldOffers = "default|";
  if (worlds.size() > 0) {
    worldOffers += worlds[0].name;
  }
  
  worldOffers += "\nadd_button|Showing: `wWorlds``|_catselect_|0.6|3529161471|\n";
  for (int i = 0; i < worlds.size(); i++) {
    worldOffers += "add_floater|"+worlds[i].name+"|"+std::to_string(getPlayersCountInWorld(worlds[i].name))+"|0.55|3529161471\n";
  }
  GamePacket p3 = packetEnd(appendString(appendString(createPacket(), "OnRequestWorldSelectMenu"), worldOffers));
  ENetPacket * packet3 = enet_packet_create(p3.data,
    p3.len,
    ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, packet3);
  delete p3.data;

  return 0;
}

void initLua () {
  L = luaL_newstate();
  luaL_openlibs(L);
  if (luaL_dofile(L, "module.lua")) {
    cout << "Module not loaded." << endl;
  }

  lua_register(L, "disconnect", disconnect);
  lua_register(L, "createPacket", GPcreatePacket);
  lua_register(L, "appendString", GPappendStr);
  lua_register(L, "packetEnd", GPpacketEnd);
  lua_register(L, "sendGamePacket", peerSendGP);
  lua_register(L, "sendWorldOffers", EXsendWorldOffers);
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
