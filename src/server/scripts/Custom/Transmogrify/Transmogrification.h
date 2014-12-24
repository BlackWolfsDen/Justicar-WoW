#include "ScriptPCH.h"

#ifndef Transmogrification_H
#define Transmogrification_H

#define TRANSMOGRIFICATION_SYSTEM_SCRIPT_NAME "custom_transmogrification_system_script"

#define QUALITY_POOR                 "cff9d9d9d"			  //GREY
#define QUALITY_NORMAL               "cffffffff"             //WHITE
#define QUALITY_UNCOMMON             "cff1eff00"             //GREEN
#define QUALITY_RARE                 "cff0070dd"             //BLUE
#define QUALITY_EPIC                 "cffa335ee"             //PURPLE
#define QUALITY_LEGENDARY            "cffff8000"             //ORANGE
#define QUALITY_ARTIFACT             "cffe6cc80"             //LIGHT YELLOW
#define QUALITY_HEIRLOOM             "cffe6cc80"			  //LIGHT YELLOW

#define PRICE_ITEM_COUNT 1
#define PRICE_ITEM_ENTRY 986504

#define TRANS_SYSTEM_MSG_ITEM_COST "|cffffffffYou will need |cff00ccff%u|cffffffffx of the item |%s|Hitem:%u:0:0:0:0:0:0:0:%u|h[%s]|h|r |cffffffffin order to use the system!"

std::map<uint64, std::map<uint32, Item*> > Items;  // Items[GUID][DISPLAY] = item
class TransmogrificationSystem : public WorldScript
{
	public:
        TransmogrificationSystem();
		std::string GetQualityColor(int32 qualityType) const; 
        bool IsSuitable(Item* pItem, Item* OLD, Player* pPlayer);
        char * GetSlotName(uint8 slot);
        uint32 HasItemCost(Player* pPlayer);
        void DestroyItemCost(Player* pPlayer);
        void CreateFakeItem(uint32 lowGUId, uint32 fakeEntry);
        void RemoveFakeItem(uint32 lowGUId);
        bool IsAlreadyFake(uint32 lowGUId);
        void SendMessageToPlayer(Player *pPlayer, const char *msgToSend, ...);
};

#define sTransmogrification (*ACE_Singleton<TransmogrificationSystem, ACE_Null_Mutex>::instance())
#endif // Transmogrification_H