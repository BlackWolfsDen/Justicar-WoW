#include "ScriptPCH.h"

class Announce_OnMapChange : public PlayerScript
{
    public:
        Announce_OnMapChange() : PlayerScript("Announce_OnMapChange") {}

    void OnPlayerEnterMap(Map* map, Player* player)
    {
    if (player->GetMapId() == 615)
    {
	 player->AddAura(68378, player);
	 player->AddItem(12584, 2);
	 ChatHandler(player).PSendSysMessage("You have all the power you could imagine to slay down Sartharion.");
    }
    if (player->GetMapId() == 608)
    {
	 player->AddAura(68378, player);
	 player->AddAura(5118, player);
	 player->AddItem(13719, 2);
	 ChatHandler(player).PSendSysMessage("You have all the power you could imagine to slay down these Monsters.");
    }
    if (player->HasAura(61254))
    {
	 player->RemoveAurasDueToSpell(61254);
	 player->TeleportTo(530, 6654.645508f, -6458.209473f, 29.259302f, 2.664963f);
    }
    }
};

void AddSC_Announce_OnMapChange()
{
    new Announce_OnMapChange();
}