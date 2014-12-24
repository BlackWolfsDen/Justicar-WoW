/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Common.h"
#include "DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "AccountMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Util.h"

bool ChatHandler::HandleRemoveCustomAuras(const char* args)
{
    Player* player = m_session->GetPlayer();

    if (player->GetMapId() == 573 || player->GetMapId() == 13 || player->HasAura(42354))
    {
        PSendSysMessage("You can't use that here.");
        SetSentErrorMessage(true);
        return false;
    }
    if (player->isInCombat() || player->isPVPArea(player->GetAreaId()))
    {
        PSendSysMessage("You can't use this in combat or in FFA PvP Areas.");
        SetSentErrorMessage(true);
        return false;
    }

    if(player->HasAura(50224) || player->HasAura(57718) || player->HasAura(51201) || player->HasAura(33341) || player->HasAura(33340) || player->HasAura(33339) || player->HasAura(33338) || player->HasAura(71947) || player->HasAura(45681) || player->HasAura(49702) || player->HasAura(49733))
    {
	player->RemoveAura(50224);
	player->RemoveAura(57718);
	player->RemoveAura(33341);
	player->RemoveAura(33340);
	player->RemoveAura(33339);
	player->RemoveAura(33338);
	player->RemoveAura(71947);
	player->RemoveAura(45681);
	player->RemoveAura(49702);
	player->RemoveAura(49733);
	player->RemoveAura(51201);
	player->RemoveAura(68085);
	ChatHandler(player).PSendSysMessage("Your auras has been removed.");
    } else {
	ChatHandler(player).PSendSysMessage("You do not have any aura that can be removed.");
    }

  return true;

}

bool ChatHandler::HandleMallCommand(const char* /*args*/)
{

        Player *chr = m_session->GetPlayer();

        if (chr->GetMap()->IsBattlegroundOrArena())
        {
        SendSysMessage(LANG_YOU_IN_BATTLEGROUND);
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInCombat())
        {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInFlight())
        {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->GetAreaId() == 2408)
        {
        SendSysMessage("You Can't use mall command in PVP MALL");
        SetSentErrorMessage(true);
        return false;
        }

        chr->ResurrectPlayer(0.5, false);

        switch(chr->GetTeam())
   {
        case ALLIANCE:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(1, -1737.410034f, 3226.96997f, 4.8107700f, 5.368655f);    // Insert Ally mall Cords here
                break;

        case HORDE:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(1, -1737.410034f, 3226.96997f, 4.8107700f, 5.368655f);    // Insert Horde mall Cords here
                break;
   }
        return true;
}

bool ChatHandler::HandleUnstuckCommand(const char* /*args*/)
{
        Player *chr = m_session->GetPlayer();

        if (chr->GetMap()->IsBattlegroundOrArena())
        {
        SendSysMessage(LANG_YOU_IN_BATTLEGROUND);
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInCombat())
        {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->isInFlight())
        {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
        }

        chr->ResurrectPlayer(0.5f);

        switch(chr->getRace())
        {
        case RACE_HUMAN:
        case RACE_NIGHTELF:
        case RACE_DWARF:
        case RACE_GNOME:
        case RACE_DRAENEI:
        case RACE_VRYKUL:
        case RACE_BROKEN:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);    // Insert Ally mall Cords here
                break;
        case RACE_NAGA:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(0, -8962.990234f, 189.345993f, 151.419006f, 1.0f);    // Insert Ally mall Cords here
                break;

        case RACE_ORC:
        case RACE_UNDEAD_PLAYER:
        case RACE_TAUREN:
        case RACE_TROLL:
        case RACE_BLOODELF:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);    // Insert Horde mall Cords here
                break;
        case RACE_GOBLIN:
		  chr->CombatStop();
		  chr->getHostileRefManager().deleteReferences();
                chr->TeleportTo(1, 1252.334473f, -4494.493164f, 22.223215f, 2.285728f);    // Insert Horde mall Cords here
                break;
        } 
        return true;
}

bool ChatHandler::HandleBerserkCommand(const char* /*args*/)              
{
        Player *chr = m_session->GetPlayer();
	 
	 if(chr->HasItemCount(31, 1))
	 {
	 if(chr->HasAura(24378))
	 {
	 SendSysMessage("You are already berserking!");
        SetSentErrorMessage(true);
        return false;
	 }
        if(chr->isInFlight())
        {
        SendSysMessage("You can't use berserk command while flying");
        SetSentErrorMessage(true);
        return false;
        }
        if (chr->GetMap()->IsBattlegroundOrArena())
        {
        SendSysMessage(LANG_YOU_IN_BATTLEGROUND);
        SetSentErrorMessage(true);
        return false;
        }
                
                {
		    chr->DestroyItemCount(31, 1, true, false);
                  chr->Dismount();
                  chr->RemoveAurasByType(SPELL_AURA_MOUNTED);
                  chr->AddAura(24378, chr);
                }
	 } else {
	 ChatHandler(chr->GetSession()).PSendSysMessage("You must buy the Berserk Token to use this command.");
	 }

                return true;
}

bool ChatHandler::HandleHelpCommand(const char* args)
{
    char* cmd = strtok((char*)args, " ");
    if (!cmd)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), cmd))
            SendSysMessage(LANG_NO_HELP_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(const char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleStartCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    if (player->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (player->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    if (player->isDead() || player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
    {
        // if player is dead and stuck, send ghost to graveyard
        player->RepopAtGraveyard();
        return true;
    }

    // cast spell Stuck
    player->CastSpell(player, 7355, false);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(const char* /*args*/)
{
    uint32 playersNum = sWorld->GetPlayerCount();
    uint32 maxPlayersNum = sWorld->GetMaxPlayerCount();
    uint32 activeClientsNum = sWorld->GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld->GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld->GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld->GetMaxQueuedSessionCount();
    std::string uptime = secsToTimeString(sWorld->GetUptime());
    uint32 updateTime = sWorld->GetUpdateTime();

    SendSysMessage(_FULLVERSION);
    PSendSysMessage(LANG_CONNECTED_PLAYERS, playersNum, maxPlayersNum);
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
    PSendSysMessage(LANG_UPTIME, uptime.c_str());
    PSendSysMessage(LANG_UPDATE_DIFF, updateTime);
    //! Can't use sWorld->ShutdownMsg here in case of console command
    if (sWorld->IsShuttingDown())
        PSendSysMessage(LANG_SHUTDOWN_TIMELEFT, secsToTimeString(sWorld->GetShutDownTimeLeft()).c_str());

    return true;
}

bool ChatHandler::HandleDismountCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    //If player is not mounted, so go out :)
    if (!player->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (player->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    player->Dismount();
    player->RemoveAurasByType(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    // save GM account without delay and output message
    if (!AccountMgr::IsPlayerAccount(m_session->GetSecurity()))
    {
        if (Player* target = getSelectedPlayer())
            target->SaveToDB();
        else
            player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save if the player has last been saved over 20 seconds ago
    uint32 save_interval = sWorld->getIntConfig(CONFIG_INTERVAL_SAVE);
    if (save_interval == 0 || (save_interval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20 * IN_MILLISECONDS))
        player->SaveToDB();

    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(const char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld->GetMotd());
    return true;
}

