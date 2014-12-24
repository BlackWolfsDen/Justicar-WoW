/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 *
 * Originally created by trinitycore, modified by nexus for better efficiency.
 */

#include "ScriptPCH.h"
#include "Chat.h"

class devtag_commandscript : public CommandScript
{
public:
    devtag_commandscript() : CommandScript("devtag_commandscript") { }

    ChatCommand* GetCommands() const
    {
	static ChatCommand CheatCommandTable[] =
        {
            { "cooldown",      SEC_PLAYER,         false, &HandleCheatCooldownCommand,   "", NULL },
            { "casttime",      SEC_PLAYER,         false, &HandleCheatCasttimeCommand,   "", NULL },
            { "power",         SEC_PLAYER,         false, &HandleCheatPowerCommand,      "", NULL },
            { "pvparea",       SEC_PLAYER,         false, &HandleCheatPvPAreaCommand,    "", NULL },
            { "help",          SEC_MODERATOR,      false, &HandleCheatHelpCommand,       "", NULL },
            { NULL,             0,                 false, NULL,                          "", NULL }
        };

	static ChatCommand HideCommandTable[] =
        {
            { "world",        SEC_PLAYER,          false, &HandleHideWorldCommand,       "", NULL },
            { "vip",          SEC_PLAYER,          false, &HandleHideVipCommand,         "", NULL },
            { NULL,             0,                 false, NULL,                          "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "castall",        SEC_PLAYER,        false,  &HandleCastAllCommand,      "", NULL },
            { "xprate",         SEC_PLAYER,        false,  &HandleXpRateCommand,       "", NULL },
            { "dev",            SEC_ADMINISTRATOR, false,  &HandleDevCommand,          "", NULL },
            { "emethtimer",     SEC_PLAYER,  	   false,  &HandleEmethCommand,        "", NULL },
            { "dantetimer",     SEC_PLAYER,  	   false,  &HandleDatraelCommand,      "", NULL },
            { "fixphase",       SEC_PLAYER,  	   false,  &HandleFixPhaseCommand,     "", NULL },
	     { "cheat",          SEC_PLAYER,	   false,  NULL,			    "", CheatCommandTable },
	     { "hide",	    SEC_PLAYER,	   false,  NULL,			    "", HideCommandTable },
            { NULL,             0,                  false,  NULL,                       "", NULL }
        };
        return commandTable;
    }

    static bool HandleCastAllCommand(ChatHandler* handler, const char *args)
    {
        if (!*args)
            return false;

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell = handler->extractSpellIdFromLink((char*)args);
        if (!spell || !sSpellMgr->GetSpellInfo(spell))
        {
            handler->PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* trig_str = strtok(NULL, " ");
        if (trig_str)
        {
            int l = strlen(trig_str);
            if (strncmp(trig_str, "triggered", l) != 0)
                return false;
        }

        bool triggered = (trig_str != NULL);

        TRINITY_READ_GUARD(HashMapHolder<Player>::LockType, *HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const& m = sObjectAccessor->GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
            itr->second->CastSpell(itr->second, spell, triggered);
        return true;
    }

	// Allow the player to set their own exp modifier
	#define MAX_XP_RATE 3
	static bool HandleXpRateCommand(ChatHandler* handler, char const* args)
	{
		Player* chr = handler->GetSession()->GetPlayer();

		if (!*args)
			return false;

		float xp_rate = atof((char*)args);

		if (xp_rate < 0 || xp_rate > MAX_XP_RATE)
		{
			handler->SendSysMessage(LANG_BAD_VALUE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		chr->SetXpRate(xp_rate);
		handler->PSendSysMessage("Your XP rate is now: %f", chr->GetXpRate());

		return true;
	}

    static bool HandleFixPhaseCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        player->SetPhaseMask(1, true);
        ChatHandler(player).PSendSysMessage("Your phase has been reset.");

        return true;
    }

    static bool HandleCheatPvPAreaCommand(ChatHandler* handler, const char* /*args*/)
    {
       Player* chr = handler->GetSession()->GetPlayer();

        if (chr->GetMap()->IsBattlegroundOrArena())
        {
        handler->SendSysMessage(LANG_YOU_IN_BATTLEGROUND);
        handler->SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInCombat())
        {
        handler->SendSysMessage(LANG_YOU_IN_COMBAT);
        handler->SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInFlight())
        {
        handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
        handler->SetSentErrorMessage(true);
        return false;
        }

        switch(chr->GetTeam())
        {
        case ALLIANCE:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(1, -3750.512451f, 1098.846294f, 151.969482f, 1.0f);    // Insert Ally mall Cords here
                break;

        case HORDE:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(1, -3750.512451f, 1098.846294f, 151.969482f, 1.0f);    // Insert Horde mall Cords here
                break;
    }
        return true;

    }

    static bool HandleEmethCommand(ChatHandler* handler, char const* args)
    {
        if (!handler)
            return false;

		Player* pPlayer = handler->GetSession()->GetPlayer();
		std::string RespawnReg = "";

		QueryResult queryResult = CharacterDatabase.PQuery("SELECT respawnTime FROM creature_respawn WHERE guid = 14034498");
		if(queryResult)
		{
			uint32 respawnUnix = queryResult->Fetch()[0].GetUInt32();
			RespawnReg = secsToTimeString(respawnUnix - time(NULL));
		} else {
			RespawnReg = "Emeth seems to be alive. Go slay him!";
		}

		ChatHandler(pPlayer->GetSession()).PSendSysMessage("Emeth will respawn in: %s", RespawnReg.c_str());

    return true;
    }

    static bool HandleDatraelCommand(ChatHandler* handler, char const* args)
    {
        if (!handler)
            return false;

		Player* pPlayer = handler->GetSession()->GetPlayer();
		std::string RespawnReg = "";

		QueryResult queryResult = CharacterDatabase.PQuery("SELECT respawnTime FROM creature_respawn WHERE guid = 1520070");
		if(queryResult)
		{
			uint32 respawnUnix = queryResult->Fetch()[0].GetUInt32();
			RespawnReg = secsToTimeString(respawnUnix - time(NULL));
		} else {
			RespawnReg = "Dante seems to be alive. Go slay him!";
		}

		ChatHandler(pPlayer->GetSession()).PSendSysMessage("Dante will respawn in: %s", RespawnReg.c_str());

    return true;
    }

	static bool HandleCheatCooldownCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(!player)
			return false;

		if(player->HasItemCount(24, 1, true))
		{
			player->DestroyItemCount(24, 1, true, true); 

			if (player)
			{
				CharacterDatabase.PExecute("REPLACE INTO character_cooldown (`guid`, `cooldown`) VALUES ('%u', '1')", player->GetGUIDLow());
				ChatHandler(player->GetSession()).PSendSysMessage("Sucess! Cooldown cheat is now active on this character.");
				player->m_CheatCooldown = true;
				return false;
			}
		}

		if((player->IsPvP() || player->IsOutdoorPvPActive() || player->GetMap()->IsBattlegroundOrArena() || player->pvpInfo.inFFAPvPArea || player->pvpInfo.inHostileArea || player->GetAreaId() == 255 || player->GetMapId() == 532 || player->GetMapId() == 631 || player->GetMapId() == 573 || player->GetAreaId() == 2397) && player->GetAreaId() != 3217)
			{ handler->SendSysMessage("You can not activate this cheat while PVP is toggled on."); return false; }

		QueryResult result = CharacterDatabase.PQuery("SELECT * FROM character_cooldown WHERE guid = '%u' AND cooldown=1", player->GetGUIDLow());
		if(!result) { handler->SendSysMessage("This cheat is not activated on your character."); return false; }

		if (!*args)
		{
			if(player->GetCommandStatus(CHEAT_COOLDOWN))
				args = "off";
			else 
				args = "on";
		}

		if(strcmp(args, "on") == 0)
		{
			player->m_CheatCooldown = true;
			handler->SendSysMessage("Cooldown cheat enabled.");
			//player->ClearAllCooldowns();
			return true;
		} 
		else if(strcmp(args, "off") == 0)
		{
			player->m_CheatCooldown = false;
			handler->SendSysMessage("Cooldown cheat disabled");
			return true;
		} 

		else
			return false;		
		return true;
	}

	static bool HandleCheatCasttimeCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(!player)
			return false;
		
		if(player->HasItemCount(25, 1, true))
		{
			player->DestroyItemCount(25, 1, true, true); 

			if (player)
			{
				CharacterDatabase.PExecute("REPLACE INTO character_casttime (`guid`, `casttime`) VALUES ('%u', '1')", player->GetGUIDLow());
				ChatHandler(player->GetSession()).PSendSysMessage("Sucess! Casttime cheat is now active on this character.");
				player->m_CheatCastTime = true;
				return false;
			}
		}

		if((player->IsPvP() || player->IsOutdoorPvPActive() || player->GetMap()->IsBattlegroundOrArena() || player->pvpInfo.inFFAPvPArea || player->pvpInfo.inHostileArea || player->GetAreaId() == 255 || player->GetMapId() == 532 || player->GetMapId() == 573 || player->GetAreaId() == 2397 || player->GetMapId() == 631) && player->GetAreaId() != 3217)
			{ handler->SendSysMessage("You can not activate this cheat while PVP is toggled on."); return false; }

		QueryResult result = CharacterDatabase.PQuery("SELECT * FROM character_casttime WHERE guid = '%u' AND casttime=1", player->GetGUIDLow());
		if(!result) { handler->SendSysMessage("This cheat is not activated on your character."); return false; }

		if (!*args)
		{
			if(player->GetCommandStatus(CHEAT_CASTTIME))
				args = "off";
			else 
				args = "on";
		}

		if(strcmp(args, "on") == 0)
		{
			player->m_CheatCastTime = true;
			handler->SendSysMessage("Casttime cheat enabled.");
			return true;
		} 
		else if(strcmp(args, "off") == 0)
		{
			player->m_CheatCastTime = false;
			handler->SendSysMessage("Casttime cheat disabled");
			return true;
		} 

		else
			return false;		
		return false;
	}

	static bool HandleHideWorldCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(!player)
			return false;

	    std::string argstr = (char*)args;

	    if (!*args)
	    {
               argstr = (player->GetCommandStatus(HIDE_WORLD)) ? "off" : "on";
	    }

	    if (argstr == "off")
	    {
		    player->m_hideWorld = false;
		    handler->SendSysMessage("You have now hidden the world channel.");
		    return true;
	    }
	    else if (argstr == "on")
	    {
		    player->m_hideWorld = true;
		    handler->SendSysMessage("You can now see the world channel.");
		    return true;
	    }
	
		return false;
	}

	static bool HandleHideVipCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(!player)
			return false;

	    std::string argstr = (char*)args;

	    if (!*args)
	    {
               argstr = (player->GetCommandStatus(HIDE_VIP)) ? "off" : "on";
	    }

	    if (argstr == "off")
	    {
		    player->m_hideVip = false;
		    handler->SendSysMessage("You have now hidden the vip channel.");
		    return true;
	    }
	    else if (argstr == "on")
	    {
		    player->m_hideVip = true;
		    handler->SendSysMessage("You can now see the vip channel.");
		    return true;
	    } 
	
		return false;
	}

	static bool HandleCheatPowerCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(!player)
			return false;

		if(player->HasItemCount(26, 1, true))
		{
			player->DestroyItemCount(26, 1, true, true); 

			if (player)
			{
				CharacterDatabase.PExecute("REPLACE INTO character_cheatpower (`guid`, `cheatpower`) VALUES ('%u', '1')", player->GetGUIDLow());
				ChatHandler(player->GetSession()).PSendSysMessage("Sucess! Power cheat is now active on this character.");
	  		       handler->GetSession()->GetPlayer()->m_cheatPower = true;
				return false;
			}
		}

		if((player->IsPvP() || player->IsOutdoorPvPActive() || player->GetMap()->IsBattlegroundOrArena() || player->pvpInfo.inFFAPvPArea || player->pvpInfo.inHostileArea || player->GetAreaId() == 255 || player->GetMapId() == 532 || player->GetMapId() == 573 || player->GetAreaId() == 2397 || player->GetMapId() == 631) && player->GetAreaId() != 3217)
			{ handler->SendSysMessage("You can not activate this cheat while PVP is toggled on."); return false; }

		QueryResult result = CharacterDatabase.PQuery("SELECT * FROM character_cheatpower WHERE guid = '%u' AND cheatpower=1", player->GetGUIDLow());
		if(!result) { handler->SendSysMessage("This cheat is not activated on your character."); return false; }

	    std::string argstr = (char*)args;

	    if (!*args)
	    {
               argstr = (player->GetCommandStatus(CHEAT_POWER)) ? "off" : "on";
	    }

	    if (argstr == "off")
	    {
		    player->m_cheatPower = false;
		    handler->SendSysMessage("Powercheat is OFF. You need mana/rage/energy to use spells.");
		    return true;
	    }
	    else if (argstr == "on")
	    {
		    player->m_cheatPower = true;
		    handler->SendSysMessage("Powercheat is ON. Don't need mana/rage/energy to use spells.");
		    return true;
	    } 
	
		return false;
	}

	static bool HandleCheatHelpCommand(ChatHandler* handler, char const* args)
	{

		return true;
	}

    static bool HandleDevCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
        {
            if (handler->GetSession()->GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DEVELOPER))
                args = "off";
            else
                args = "on";
            return true;
        }

        std::string argstr = (char*)args;

        if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetDevFlag(true);
            handler->GetSession()->SendNotification(LANG_DEV_ON);
            handler->GetSession()->GetPlayer()->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("stoplog");
#endif
            return true;
        }

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetDevFlag(false);
            handler->GetSession()->SendNotification(LANG_DEV_OFF);
            handler->GetSession()->GetPlayer()->UpdateTriggerVisibility();
#ifdef _DEBUG_VMAPS
            VMAP::IVMapManager* vMapManager = VMAP::VMapFactory::createOrGetVMapManager();
            vMapManager->processCommand("startlog");
#endif
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }
};

void AddSC_devtag_commandscript()
{
    new devtag_commandscript();
}
