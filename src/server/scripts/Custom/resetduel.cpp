#include "ScriptPCH.h"

class Reset_OnDuelEnd : public PlayerScript
{
    public:
        Reset_OnDuelEnd() : PlayerScript("Reset_OnDuelEnd") {}

    void OnDuelEnd(Player *winner, Player *looser, DuelCompleteType type)
    {
                        winner->GetSession()->SendNotification("You just defeated %s! in a duel!", looser->GetName());
                        winner->RemoveArenaSpellCooldowns();
                        winner->SetHealth(winner->GetMaxHealth());
                        looser->GetSession()->SendNotification("%s defeated you in a duel!", winner->GetName());
                        looser->SetHealth(looser->GetMaxHealth());
	                 if (winner->getPowerType() == POWER_MANA) 
       	          	winner->SetPower(POWER_MANA, winner->GetMaxPower(POWER_MANA));
            		   if (looser->getPowerType() == POWER_MANA) 
                	   	looser->SetPower(POWER_MANA, looser->GetMaxPower(POWER_MANA));
    }
};

void AddSC_Reset()
{
    new Reset_OnDuelEnd;
}
