#pragma once

#ifdef _WIN32
#include <windows.h>
typedef __int8 __int8_t;
typedef __int16 __int16_t;
#elif __APPLE__ || __linux__
typedef unsigned int DWORD;
#endif
typedef unsigned char BYTE;

using namespace std;
using json = nlohmann::json;

char _getch();
string GetLastError();
unsigned long _byteswap_ulong(__uint64_t val);

extern ENetHost * server;
extern int cId;
extern BYTE* itemsDat;
extern int itemsDatSize;

extern int serverHash;

string hashPassword(string password);

bool verifyPassword(string password, string hash);

void sendData(ENetPeer* peer, int num, char* data, int len);

int getPacketId(char* data);

char* getPacketData(char* data);

string text_encode(char* text);

int ch2n(char x);

char* GetTextPointerFromPacket(ENetPacket* packet);

BYTE* GetStructPointerFromTankPacket(ENetPacket* packet);

int GetMessageTypeFromPacket(ENetPacket* packet);

vector<string> explode(const string &delimiter, const string &str);

struct GamePacket
{
	BYTE* data;
	int len;
	int indexes;
};

GamePacket appendFloat(GamePacket p, float val);

GamePacket appendFloat(GamePacket p, float val, float val2);

GamePacket appendFloat(GamePacket p, float val, float val2, float val3);

GamePacket appendInt(GamePacket p, int val);

GamePacket appendIntx(GamePacket p, int val);

GamePacket appendString(GamePacket p, string str);

GamePacket createPacket();

GamePacket packetEnd(GamePacket p);

struct InventoryItem {
	__int16_t itemID;
	__int8_t itemCount;
};

struct PlayerInventory {
	vector<InventoryItem> items;
	int inventorySize = 100;
};

#define cloth0 cloth_hair
#define cloth1 cloth_shirt
#define cloth2 cloth_pants
#define cloth3 cloth_feet
#define cloth4 cloth_face
#define cloth5 cloth_hand
#define cloth6 cloth_back
#define cloth7 cloth_mask
#define cloth8 cloth_necklace

struct PlayerInfo {
	bool isIn = false;
	int netID;
	bool haveGrowId = false;
	string tankIDName = "";
	string tankIDPass = "";
	string requestedName = "";
	string rawName = "";
	string displayName = "";
	string country = "";
	int adminLevel = 0;
	string currentWorld = "EXIT";
	bool radio = true;
	int x;
	int y;
	int x1;
	int y1;
	bool isRotatedLeft = false;

	bool isUpdating = false;
	bool joinClothesUpdated = false;
	
	bool taped = false;

	int cloth_hair = 0; // 0
	int cloth_shirt = 0; // 1
	int cloth_pants = 0; // 2
	int cloth_feet = 0; // 3
	int cloth_face = 0; // 4
	int cloth_hand = 0; // 5
	int cloth_back = 0; // 6
	int cloth_mask = 0; // 7
	int cloth_necklace = 0; // 8

	bool canWalkInBlocks = false; // 1
	bool canDoubleJump = false; // 2
	bool isInvisible = false; // 4
	bool noHands = false; // 8
	bool noEyes = false; // 16
	bool noBody = false; // 32
	bool devilHorns = false; // 64
	bool goldenHalo = false; // 128
	bool isFrozen = false; // 2048
	bool isCursed = false; // 4096
	bool isDuctaped = false; // 8192
	bool haveCigar = false; // 16384
	bool isShining = false; // 32768
	bool isZombie = false; // 65536
	bool isHitByLava = false; // 131072
	bool haveHauntedShadows = false; // 262144
	bool haveGeigerRadiation = false; // 524288
	bool haveReflector = false; // 1048576
	bool isEgged = false; // 2097152
	bool havePineappleFloag = false; // 4194304
	bool haveFlyingPineapple = false; // 8388608
	bool haveSuperSupporterName = false; // 16777216
	bool haveSupperPineapple = false; // 33554432
	bool isGhost = false;
	//bool 
	int skinColor = 0x8295C3FF; //normal SKin color like gt!

	PlayerInventory inventory;

	long long int lastSB = 0;
};

int getState(PlayerInfo* info);

struct WorldItem {
	__int16_t foreground = 0;
	__int16_t background = 0;
	int breakLevel = 0;
	long long int breakTime = 0;
	bool water = false;
	bool fire = false;
	bool glue = false;
	bool red = false;
	bool green = false;
	bool blue = false;
};

struct WorldInfo {
	int width = 100;
	int height = 60;
	string name = "TEST";
	WorldItem* items;
	string owner = "";
	bool isPublic=false;
};

WorldInfo generateWorld(string name, int width, int height);

class PlayerDB {
public:
	static string getProperName(string name);
	static string fixColors(string text);
	static int playerLogin(ENetPeer* peer, string username, string password);
	static int playerRegister(string username, string password, string passwordverify, string email, string discord);
};

struct AWorld {
	WorldInfo* ptr;
	WorldInfo info;
	int id;
};

class WorldDB {
public:
	WorldInfo get(string name);
	AWorld get2(string name);
	void flush(WorldInfo info);
	void flush2(AWorld info);
	void save(AWorld info);
	void saveAll();
	void saveRedundant();
	vector<WorldInfo> getRandomWorlds();
	WorldDB();
private:
	vector<WorldInfo> worlds;
};

void sendConsoleMsg(ENetPeer* peer, string message);

string getStrUpper(string txt);

extern WorldDB worldDB;

void saveAllWorlds();

WorldInfo* getPlyersWorld(ENetPeer* peer);

struct PlayerMoving {
	int packetType;
	int netID;
	float x;
	float y;
	int characterState;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int punchX;
	int punchY;

};

enum ClothTypes {
	HAIR,
	SHIRT,
	PANTS,
	FEET,
	FACE,
	HAND,
	BACK,
	MASK,
	NECKLACE,
	NONE
};

enum BlockTypes {
	FOREGROUND,
	BACKGROUND,
	CONSUMABLE,
	SEED,
	PAIN_BLOCK,
	BEDROCK,
	MAIN_DOOR,
	SIGN,
	DOOR,
	CLOTHING,
	FIST,
	UNKNOWN
};

struct ItemDefinition {
	int id;
	string name;
	int rarity;
	int breakHits;
	int growTime;
	ClothTypes clothType;
	BlockTypes blockType;
	string description = "This item has no description.";
};

extern vector<ItemDefinition> itemDefs;

struct DroppedItem { // TODO
	int id;
	int uid;
	int count;
};

extern vector<DroppedItem> droppedItems;

ItemDefinition getItemDef(int id);

void craftItemDescriptions();

void buildItemsDatabase();

struct Admin {
	string username;
	string password;
	int level = 0;
	long long int lastSB = 0;
};

extern vector<Admin> admins;

void addAdmin(string username, string password, int level);

int getAdminLevel(string username, string password);

bool canSB(string username, string password);

bool canClear(string username, string password);

bool isSuperAdmin(string username, string password);

bool isHere(ENetPeer* peer, ENetPeer* peer2);

void sendInventory(ENetPeer* peer, PlayerInventory inventory);

BYTE* packPlayerMoving(PlayerMoving* dataStruct);

PlayerMoving* unpackPlayerMoving(BYTE* data);

void SendPacket(int a1, string a2, ENetPeer* enetPeer);

void SendPacketRaw(int a1, void *packetData, size_t packetDataSize, void *a4, ENetPeer* peer, int packetFlag);

void onPeerConnect(ENetPeer* peer);

void updateAllClothes(ENetPeer* peer);

void sendClothes(ENetPeer* peer);

void sendPData(ENetPeer* peer, PlayerMoving* data);

int getPlayersCountInWorld(string name);

void sendRoulete(ENetPeer* peer, int x, int y);

void sendNothingHappened(ENetPeer* peer, int x, int y);

void sendTileUpdate(int x, int y, int tile, int causedBy, ENetPeer* peer);

void sendPlayerLeave(ENetPeer* peer, PlayerInfo* player);

void sendChatMessage(ENetPeer* peer, int netID, string message);

void sendWho(ENetPeer* peer);

void sendWorld(ENetPeer* peer, WorldInfo* worldInfo);

void sendAction(ENetPeer* peer, int netID, string action);

void sendDrop(ENetPeer* peer, int netID, int x, int y, int item, int count, BYTE specialEffect);

void sendState(ENetPeer* peer);

void sendWorldOffers(ENetPeer* peer);