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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "TicketMgr.h"
#include "Player.h"
#include "AccountMgr.h"
#include "Opcodes.h"
#include "Chat.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "Util.h"
#include "Group.h"

#ifdef _DEBUG_VMAPS
#include "VMapFactory.h"
#endif

	bool ChatHandler::HandleEarnVIPCommand(const char* args)
	{
		Player* player = m_session->GetPlayer();

		if(player->HasItemCount(22, 1, false))
		{
			player->DestroyItemCount(22, 1, true, false); 

			if (player)
			{
				// check online security
				if (HasLowerSecurity(player, 0))
					return false;

				LoginDatabase.PExecute("REPLACE INTO account_access (`id`, `gmlevel`, `RealmID`) VALUES ('%u', '1', '-1')", player->GetSession()->GetAccountId());
				LoginDatabase.PExecute("REPLACE INTO account_premium (`id`, `setdate`, `unsetdate`, `premium_type`, `active`) VALUES ('%u', '0', '0', '1', '1')", player->GetSession()->GetAccountId());
				ChatHandler(player->GetSession()).PSendSysMessage("Sucess! Restart WoW and you will be a VIP!");
			}
		} else { 
		ChatHandler(player->GetSession()).PSendSysMessage("You need to buy VIP token  to use this command!"); 
		}
		return true;
	}

	bool ChatHandler::HandleEventCoinsCommand(const char* args)
	{
		Player* target = m_session->GetPlayer();
		ItemPosCountVec dest;	
		Item* pItem;
		if(target->HasItemCount(16, 1, false))
		{ 	
			target->DestroyItemCount(16, 1, true, false); 			
			if (target)
			{
				// check online security
				if (HasLowerSecurity(target, 0))
					return false;

				target->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23247, 200, 0);			
				pItem = target->StoreNewItem(dest, 23247,true,Item::GenerateItemRandomPropertyId(23247));
				target->SendNewItem(pItem, 200, true, false);     			
				dest.clear();
			}
		} else {
			ChatHandler(target->GetSession()).PSendSysMessage("You need to buy the x200 token or win an event to use this command!"); 
		}
	return true;
	}

	//**
	//VIP Character Customize Command
	//**
	bool ChatHandler::HandleVIPCharCustomizeCommand(const char* args)
	{
		Player* target = m_session->GetPlayer();
		if(target->HasItemCount(1, 1, false)) 
		{ 
			target->DestroyItemCount(1, 1, true, false); 

			if (target)
			{
				PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
				target->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
				CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '8' WHERE guid = '%u'", target->GetGUIDLow());
				ChatHandler(target->GetSession()).PSendSysMessage("Sucess! Relog and you will be able to re-customize your character!");
				target->SaveToDB();
			}
		} else { 
		ChatHandler(target->GetSession()).PSendSysMessage("You need to buy re-custom token to use this command!"); 
		}
		return true;
	}

bool ChatHandler::HandleVIPChangeRaceCommand(const char * args)
{
    Player* target = m_session->GetPlayer();

    if(target->HasItemCount(81, 1, false))
    {
	 target->DestroyItemCount(81, 1, true, false);
	 if (target)
	 {
	 // check online security
	 if (HasLowerSecurity(target, 0))
	 return false;

        PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
        target->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = %u", target->GetGUIDLow());
	 target->SaveToDB();
	 }
    } else {
	 ChatHandler(target->GetSession()).PSendSysMessage("You need to buy race change token to use this command!");
    }

    return true;
}
	//**
	//VIP Character Rename Command
	//**
	bool ChatHandler::HandleVIPCharRenameCommand(const char* args)
	{
		Player* target = m_session->GetPlayer();
		if(target->HasItemCount(6, 1, false))
		{ 	
			target->DestroyItemCount(6, 1, true, false); 			
			if (target)
			{
				// check online security
				if (HasLowerSecurity(target, 0))
					return false;

				PSendSysMessage(LANG_RENAME_PLAYER, GetNameLink(target).c_str());
				target->SetAtLoginFlag(AT_LOGIN_RENAME);
				ChatHandler(target->GetSession()).PSendSysMessage("Success! Just relog and you can change your name!");
				target->SaveToDB();
			}
		} else {
			ChatHandler(target->GetSession()).PSendSysMessage("You need to buy rename token to use this command!"); 
		}
	return true;
	}
	
	
	//**
	//VIP Character Change Faction Command
	//**
	bool ChatHandler::HandleVIPFactionChangeCommand(const char * args)
	{
		Player* target = m_session->GetPlayer();
		if(target->HasItemCount(5, 1, false))
		{ 
			target->DestroyItemCount(5, 1, true, false); 
			if(target)
			{
				PSendSysMessage(LANG_CUSTOMIZE_PLAYER, GetNameLink(target).c_str());
				target->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
				CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '64' WHERE guid = %u", target->GetGUIDLow());
				ChatHandler(target->GetSession()).PSendSysMessage("Sucess! Relog and you will be able to change your faction!");
				target->SaveToDB();
			}
		} else {
			ChatHandler(target->GetSession()).PSendSysMessage("You need to buy faction change token to use this command!");
		}
		return true;
	}

bool ChatHandler::HandleMaxLevelCommand(const char* args)
{
Player *pPlr = m_session->GetPlayer(); 
if(pPlr->getLevel() != 255) { 
if(pPlr->HasItemCount(7, 1, false)) 
{ 
pPlr->DestroyItemCount(7, 1, true, false); 
pPlr->GiveLevel(255); 
} else { 
ChatHandler(pPlr->GetSession()).PSendSysMessage("You need to buy a Maxlevel token to use this command!"); 
}
} else { 
ChatHandler(pPlr->GetSession()).PSendSysMessage("You are already level 255."); 
}
return true;
}

bool ChatHandler::HandleNameAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    std::string name("Console");
    if (WorldSession* session = GetSession())
        name = session->GetPlayer()->GetName();

    sWorld->SendWorldText(LANG_ANNOUNCE_COLOR, name.c_str(), args);
    return true;
}

bool ChatHandler::HandleGMNameAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    std::string name("Console");
    if (WorldSession* session = GetSession())
        name = session->GetPlayer()->GetName();

    sWorld->SendGMText(LANG_GM_ANNOUNCE_COLOR, name.c_str(), args);
    return true;
}

// global announce
bool ChatHandler::HandleAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    char buff[2048];
    sprintf(buff, GetTrinityString(LANG_SYSTEMMESSAGE), args);
    sWorld->SendServerMessage(SERVER_MSG_STRING, buff);
    return true;
}

// announce to logged in GMs
bool ChatHandler::HandleGMAnnounceCommand(const char* args)
{
    if (!*args)
        return false;

    sWorld->SendGMText(LANG_GM_BROADCAST, args);
    return true;
}

//notification player at the screen
bool ChatHandler::HandleNotifyCommand(const char* args)
{
    if (!*args)
        return false;

    std::string str = GetTrinityString(LANG_GLOBAL_NOTIFY);
    str += args;

    WorldPacket data(SMSG_NOTIFICATION, (str.size()+1));
    data << str;
    sWorld->SendGlobalMessage(&data);

    return true;
}

//notification GM at the screen
bool ChatHandler::HandleGMNotifyCommand(const char* args)
{
    if (!*args)
        return false;

    std::string str = GetTrinityString(LANG_GM_NOTIFY);
    str += args;

    WorldPacket data(SMSG_NOTIFICATION, (str.size()+1));
    data << str;
    sWorld->SendGlobalGMMessage(&data);

    return true;
}

//Summon Player
bool ChatHandler::HandleSummonCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    Player* _player = m_session->GetPlayer();
    if (target == _player || target_guid == _player->GetGUID())
    {
        PSendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    // check offline security
    if (HasLowerSecurity(target, 0))
    {
        PSendSysMessage("You can't summon offline players.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!target->GetCommandStatus(TOGGLE_SUMMON))
    {
          PSendSysMessage(LANG_CANNOT_SUMMON_PLAYER, GetNameLink(target).c_str());
          SetSentErrorMessage(true);
          return false;
    }

    if (target->GetAreaId() == 46 || target->GetAreaId() == 3457 || target->GetAreaId() == 307 || target->GetAreaId() == 977 || target->GetAreaId() == 2177 || target->GetAreaId() == 2240 || target->GetAreaId() == 406 || target->GetAreaId() == 2318 || target->GetAreaId() == 1741 || target->GetAreaId() == 150 || target->GetAreaId() == 2104 || target->GetAreaId() == 2103 || target->GetAreaId() == 1519 || target->GetAreaId() == 717 || target->GetMapId() == 13  || target->GetAreaId() == 856 || target->GetMapId() == 573)
    {
          PSendSysMessage("You can't summon players from that location. That would be cheating!");
          SetSentErrorMessage(true);
          return false;
    }

    if (target)
    {
        Player* player = m_session->GetPlayer();

        std::string nameLink = playerLink(target_name);
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        if (target->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, nameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

  	 if (target->isGameMaster())
  	 {
  	     PSendSysMessage(LANG_PLAYER_IS_GAMEMASTER);
      	     SetSentErrorMessage(true);
  	     return false;
  	 }

        Map* map = m_session->GetPlayer()->GetMap();

        if (target->GetAreaId() == 46 || target->GetAreaId() == 3457 || target->GetAreaId() == 307 || target->GetAreaId() == 977 || target->GetAreaId() == 2177 || target->GetAreaId() == 2240 || target->GetAreaId() == 406 || target->GetAreaId() == 2318 || target->GetAreaId() == 1741 || target->GetAreaId() == 150 || target->GetAreaId() == 2104 || target->GetAreaId() == 2103 || target->GetAreaId() == 1519 || target->GetAreaId() == 717 || target->GetMapId() == 13  || target->GetAreaId() == 856 || target->GetMapId() == 573)
        {
            PSendSysMessage("You can't summon players from that location. That would be cheating!");
            SetSentErrorMessage(true);
            return false;
        }

        if (map->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!_player->isGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, nameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (target->GetBattlegroundId() && m_session->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
                target->LeaveBattleground(false); // Note: should be changed so target gets no Deserter debuff

            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            target->SetBattlegroundId(m_session->GetPlayer()->GetBattlegroundId(), m_session->GetPlayer()->GetBattlegroundTypeId());
            // remember current position as entry point for return at bg end teleportation
            if (!target->GetMap()->IsBattlegroundOrArena())
                target->SetBattlegroundEntryPoint();
        }
        else if (map->IsDungeon())
        {
            Map* cMap = target->GetMap();

            if (cMap->Instanceable() && cMap->GetInstanceId() != map->GetInstanceId())
                target->UnbindInstance(map->GetInstanceId(), target->GetDungeonDifficulty(), true);

            // we are in instance, and can summon only player in our group with us as lead
            if (!m_session->GetPlayer()->GetGroup() || !target->GetGroup() ||
                (target->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
                (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()))
                // the last check is a bit excessive, but let it be, just in case
            {
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, nameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), "");
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_SUMMONED_BY, playerLink(_player->GetName()).c_str());

        // stop flight if need
        if (target->isInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            target->SaveRecallPosition();

        // before GM
        float x, y, z;
        m_session->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
        target->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
        target->SetPhaseMask(m_session->GetPlayer()->GetPhaseMask(), true);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), GetTrinityString(LANG_OFFLINE));

        // in point where GM stay
        Player::SavePositionInDB(m_session->GetPlayer()->GetMapId(),
            m_session->GetPlayer()->GetPositionX(),
            m_session->GetPlayer()->GetPositionY(),
            m_session->GetPlayer()->GetPositionZ(),
            m_session->GetPlayer()->GetOrientation(),
            m_session->GetPlayer()->GetZoneId(),
            target_guid);
    }

    return true;
}

//Summon Player
bool ChatHandler::HandleGMSummonCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    Player* _player = m_session->GetPlayer();
    if (target == _player || target_guid == _player->GetGUID())
    {
        PSendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    if (target)
    {
        Player* player = m_session->GetPlayer();

        std::string nameLink = playerLink(target_name);
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        if (target->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, nameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        Map* map = m_session->GetPlayer()->GetMap();
        if (map->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!_player->isGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, nameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (target->GetBattlegroundId() && m_session->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
                target->LeaveBattleground(false); // Note: should be changed so target gets no Deserter debuff

            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            target->SetBattlegroundId(m_session->GetPlayer()->GetBattlegroundId(), m_session->GetPlayer()->GetBattlegroundTypeId());
            // remember current position as entry point for return at bg end teleportation
            if (!target->GetMap()->IsBattlegroundOrArena())
                target->SetBattlegroundEntryPoint();
        }
        else if (map->IsDungeon())
        {
            Map* cMap = target->GetMap();

            if (cMap->Instanceable() && cMap->GetInstanceId() != map->GetInstanceId())
                target->UnbindInstance(map->GetInstanceId(), target->GetDungeonDifficulty(), true);

            // we are in instance, and can summon only player in our group with us as lead
            if (!m_session->GetPlayer()->GetGroup() || !target->GetGroup() ||
                (target->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
                (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()))
                // the last check is a bit excessive, but let it be, just in case
            {
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, nameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), "");
        if (needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_SUMMONED_BY, playerLink(_player->GetName()).c_str());

        // stop flight if need
        if (target->isInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            target->SaveRecallPosition();

        // before GM
        float x, y, z;
        m_session->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
        target->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
        target->SetPhaseMask(m_session->GetPlayer()->GetPhaseMask(), true);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), GetTrinityString(LANG_OFFLINE));

        // in point where GM stay
        Player::SavePositionInDB(m_session->GetPlayer()->GetMapId(),
            m_session->GetPlayer()->GetPositionX(),
            m_session->GetPlayer()->GetPositionY(),
            m_session->GetPlayer()->GetPositionZ(),
            m_session->GetPlayer()->GetOrientation(),
            m_session->GetPlayer()->GetZoneId(),
            target_guid);
    }

    return true;
}

//Teleport to Player
bool ChatHandler::HandleAppearCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    Player* _player = m_session->GetPlayer();
    if (target == _player || target_guid == _player->GetGUID())
    {
        SendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    // check offline security
    if (HasLowerSecurity(target, 0))
    {
        PSendSysMessage("You can't appear offline players.");
        SetSentErrorMessage(true);
        return false;
    }

        if (!target->GetCommandStatus(TOGGLE_APPEAR))
        {
        	PSendSysMessage(LANG_CANNOT_APPEAR_TO_PLAYER, GetNameLink(target).c_str());
        	SetSentErrorMessage(true);
        	return false;
        }

  	 if (target->isGameMaster())
  	 {
  	 	PSendSysMessage(LANG_PLAYER_IS_GAMEMASTER);
      	 	SetSentErrorMessage(true);
  	 	return false;
  	 }

        if (target->GetAreaId() == 46 || target->GetAreaId() == 307 || target->GetAreaId() == 977 || target->GetAreaId() == 2177 || target->GetAreaId() == 2240 || target->GetAreaId() == 406 || target->GetAreaId() == 2318 || target->GetAreaId() == 1741 || target->GetAreaId() == 150 || target->GetAreaId() == 2104 || target->GetAreaId() == 2103 || target->GetAreaId() == 1519 || target->GetAreaId() == 717 || target->GetMapId() == 13 || target->GetAreaId() == 856 || target->GetAreaId() == 3457 || target->GetMapId() == 573)
        {
        	PSendSysMessage("You can't appear players at that location. That would be cheating!");
        	SetSentErrorMessage(true);
        	return false;
        }

    if (target)
    {
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        std::string chrNameLink = playerLink(target_name);

        Map* cMap = target->GetMap();

  	 if (target->isGameMaster())
  	 {
  	 	PSendSysMessage(LANG_PLAYER_IS_GAMEMASTER);
      	 	SetSentErrorMessage(true);
  	 	return false;
  	 }

        if (target->GetAreaId() == 46 || target->GetAreaId() == 307 || target->GetAreaId() == 977 || target->GetAreaId() == 2177 || target->GetAreaId() == 2240 || target->GetAreaId() == 406 || target->GetAreaId() == 2318 || target->GetAreaId() == 1741 || target->GetAreaId() == 150 || target->GetAreaId() == 2104 || target->GetAreaId() == 2103 || target->GetAreaId() == 1519 || target->GetAreaId() == 717 || target->GetMapId() == 13 || target->GetAreaId() == 856 || target->GetAreaId() == 3457 || target->GetMapId() == 573)
        {
        	PSendSysMessage("You can't appear players at that location. That would be cheating!");
        	SetSentErrorMessage(true);
        	return false;
        }

        if (cMap->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!_player->isGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, chrNameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            _player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
            // remember current position as entry point for return at bg end teleportation
            if (!_player->GetMap()->IsBattlegroundOrArena())
                _player->SetBattlegroundEntryPoint();
        }
        else if (cMap->IsDungeon())
        {
            // we have to go to instance, and can go to player only if:
            //   1) we are in his group (either as leader or as member)
            //   2) we are not bound to any group and have GM mode on
            if (_player->GetGroup())
            {
                // we are in group, we can go only if we are in the player group
                if (_player->GetGroup() != target->GetGroup())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, chrNameLink.c_str());
                    SetSentErrorMessage(true);
                    return false;
                }
            }
            else
            {
                // we are not in group, let's verify our GM mode
                if (!_player->isGameMaster())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, chrNameLink.c_str());
                    SetSentErrorMessage(true);
                    return false;
                }
            }

            // if the player or the player's group is bound to another instance
            // the player will not be bound to another one
            InstancePlayerBind* pBind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty(cMap->IsRaid()));
            if (!pBind)
            {
                Group* group = _player->GetGroup();
                // if no bind exists, create a solo bind
                InstanceGroupBind* gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                if (!gBind)
                    if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
                        _player->BindToInstance(save, !save->CanReset());
            }

            if (cMap->IsRaid())
                _player->SetRaidDifficulty(target->GetRaidDifficulty());
            else
                _player->SetDungeonDifficulty(target->GetDungeonDifficulty());
        }

        PSendSysMessage(LANG_APPEARING_AT, chrNameLink.c_str());

        // stop flight if need
        if (_player->isInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        // to point to see at target with same orientation
        float x, y, z;
        target->GetContactPoint(_player, x, y, z);

	 ChatHandler(target->GetSession()).PSendSysMessage(LANG_APPEARING_AT_YOUR_LOCATION, GetNameLink(_player).c_str());
        _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);
        _player->SetPhaseMask(target->GetPhaseMask(), true);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_APPEARING_AT, nameLink.c_str());

        // to point where player stay (if loaded)
        float x, y, z, o;
        uint32 map;
        bool in_flight;
        if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, target_guid))
            return false;

        // stop flight if need
        if (_player->isInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

	 ChatHandler(target->GetSession()).PSendSysMessage(LANG_APPEARING_AT_YOUR_LOCATION, GetNameLink(_player).c_str());
        _player->TeleportTo(map, x, y, z, _player->GetOrientation());
    }

    return true;
}

//Teleport to Player
bool ChatHandler::HandleGMAppearCommand(const char* args)
{
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    Player* _player = m_session->GetPlayer();
    if (target == _player || target_guid == _player->GetGUID())
    {
        SendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    if (target)
    {
        // check online security
        if (HasLowerSecurity(target, 0))
            return false;

        std::string chrNameLink = playerLink(target_name);

        Map* cMap = target->GetMap();
        if (cMap->IsBattlegroundOrArena())
        {
            // only allow if gm mode is on
            if (!_player->isGameMaster())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, chrNameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            // if both players are in different bgs
            else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            _player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
            // remember current position as entry point for return at bg end teleportation
            if (!_player->GetMap()->IsBattlegroundOrArena())
                _player->SetBattlegroundEntryPoint();
        }
        else if (cMap->IsDungeon())
        {
            // we have to go to instance, and can go to player only if:
            //   1) we are in his group (either as leader or as member)
            //   2) we are not bound to any group and have GM mode on
            if (_player->GetGroup())
            {
                // we are in group, we can go only if we are in the player group
                if (_player->GetGroup() != target->GetGroup())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, chrNameLink.c_str());
                    SetSentErrorMessage(true);
                    return false;
                }
            }
            else
            {
                // we are not in group, let's verify our GM mode
                if (!_player->isGameMaster())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, chrNameLink.c_str());
                    SetSentErrorMessage(true);
                    return false;
                }
            }

            // if the player or the player's group is bound to another instance
            // the player will not be bound to another one
            InstancePlayerBind* pBind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty(cMap->IsRaid()));
            if (!pBind)
            {
                Group* group = _player->GetGroup();
                // if no bind exists, create a solo bind
                InstanceGroupBind* gBind = group ? group->GetBoundInstance(target) : NULL;                // if no bind exists, create a solo bind
                if (!gBind)
                    if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
                        _player->BindToInstance(save, !save->CanReset());
            }

            if (cMap->IsRaid())
                _player->SetRaidDifficulty(target->GetRaidDifficulty());
            else
                _player->SetDungeonDifficulty(target->GetDungeonDifficulty());
        }

        PSendSysMessage(LANG_APPEARING_AT, chrNameLink.c_str());

        // stop flight if need
        if (_player->isInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        // to point to see at target with same orientation
        float x, y, z;
        target->GetContactPoint(_player, x, y, z);

        _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);
        _player->SetPhaseMask(target->GetPhaseMask(), true);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(NULL, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_APPEARING_AT, nameLink.c_str());

        // to point where player stay (if loaded)
        float x, y, z, o;
        uint32 map;
        bool in_flight;
        if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, target_guid))
            return false;

        // stop flight if need
        if (_player->isInFlight())
        {
            _player->GetMotionMaster()->MovementExpired();
            _player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            _player->SaveRecallPosition();

        _player->TeleportTo(map, x, y, z, _player->GetOrientation());
    }

    return true;
}

// Teleport player to last position
bool ChatHandler::HandleRecallCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    if (target->IsBeingTeleported())
    {
        PSendSysMessage(LANG_IS_TELEPORTED, GetNameLink(target).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    // stop flight if need
    if (target->isInFlight())
    {
        target->GetMotionMaster()->MovementExpired();
        target->CleanupAfterTaxiFlight();
    }

    target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
    return true;
}

//Enable On\OFF all taxi paths
bool ChatHandler::HandleTaxiCheatCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    std::string argstr = (char*)args;

    Player* chr = getSelectedPlayer();

    if (!chr)
        chr = m_session->GetPlayer();
    else if (HasLowerSecurity(chr, 0)) // check online security
        return false;

    if (argstr == "on")
    {
        chr->SetTaxiCheater(true);
        PSendSysMessage(LANG_YOU_GIVE_TAXIS, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, GetNameLink().c_str());
        return true;
    }

    if (argstr == "off")
    {
        chr->SetTaxiCheater(false);
        PSendSysMessage(LANG_YOU_REMOVE_TAXIS, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_TAXIS_REMOVED, GetNameLink().c_str());

        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleLookupAreaCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr (namepart, wnamepart))
        return false;

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // converting string that we try to find to lower case
    wstrToLower (wnamepart);

    // Search in AreaTable.dbc
    for (uint32 areaflag = 0; areaflag < sAreaStore.GetNumRows(); ++areaflag)
    {
        AreaTableEntry const* areaEntry = sAreaStore.LookupEntry(areaflag);
        if (areaEntry)
        {
            int loc = GetSessionDbcLocale();
            std::string name = areaEntry->area_name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo (name, wnamepart))
            {
                loc = 0;
                for (; loc < TOTAL_LOCALES; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = areaEntry->area_name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo (name, wnamepart))
                        break;
                }
            }

            if (loc < TOTAL_LOCALES)
            {
                if (maxResults && count++ == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                // send area in "id - [name]" format
                std::ostringstream ss;
                if (m_session)
                    ss << areaEntry->ID << " - |cffffffff|Harea:" << areaEntry->ID << "|h[" << name << ' ' << localeNames[loc]<< "]|h|r";
                else
                    ss << areaEntry->ID << " - " << name << ' ' << localeNames[loc];

                SendSysMessage(ss.str().c_str());

                if (!found)
                    found = true;
            }
        }
    }

    if (!found)
        SendSysMessage(LANG_COMMAND_NOAREAFOUND);

    return true;
}

//Find tele in game_tele order by name
bool ChatHandler::HandleLookupTeleCommand(const char * args)
{
    if (!*args)
    {
        SendSysMessage(LANG_COMMAND_TELE_PARAMETER);
        SetSentErrorMessage(true);
        return false;
    }

    char const* str = strtok((char*)args, " ");
    if (!str)
        return false;

    std::string namepart = str;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    std::ostringstream reply;
    uint32 count = 0;
    uint32 maxResults = sWorld->getIntConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);
    bool limitReached = false;

    GameTeleContainer const & teleMap = sObjectMgr->GetGameTeleMap();
    for (GameTeleContainer::const_iterator itr = teleMap.begin(); itr != teleMap.end(); ++itr)
    {
        GameTele const* tele = &itr->second;

        if (tele->wnameLow.find(wnamepart) == std::wstring::npos)
            continue;

        if (maxResults && count++ == maxResults)
        {
            limitReached = true;
            break;
        }

        if (m_session)
            reply << "  |cffffffff|Htele:" << itr->first << "|h[" << tele->name << "]|h|r\n";
        else
            reply << "  " << itr->first << ' ' << tele->name << "\n";
    }

    if (reply.str().empty())
        SendSysMessage(LANG_COMMAND_TELE_NOLOCATION);
    else
        PSendSysMessage(LANG_COMMAND_TELE_LOCATION, reply.str().c_str());

    if (limitReached)
        PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);

    return true;
}

//Enable\Dissable accept whispers (for GM)
bool ChatHandler::HandleWhispersCommand(const char* args)
{
    if (!*args)
    {
        PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, m_session->GetPlayer()->isAcceptWhispers() ?  GetTrinityString(LANG_ON) : GetTrinityString(LANG_OFF));
        return true;
    }

    std::string argstr = (char*)args;
    // whisper on
    if (argstr == "on")
    {
        m_session->GetPlayer()->SetAcceptWhispers(true);
        SendSysMessage(LANG_COMMAND_WHISPERON);
        return true;
    }

    // whisper off
    if (argstr == "off")
    {
        // Remove all players from the Gamemaster's whisper whitelist
        m_session->GetPlayer()->ClearWhisperWhiteList();
        m_session->GetPlayer()->SetAcceptWhispers(false);
        SendSysMessage(LANG_COMMAND_WHISPEROFF);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    SetSentErrorMessage(true);
    return false;
}

//Save all players in the world
bool ChatHandler::HandleSaveAllCommand(const char* /*args*/)
{
    sObjectAccessor->SaveAllPlayers();
    SendSysMessage(LANG_PLAYERS_SAVED);
    return true;
}

//Send mail by command
bool ChatHandler::HandleSendMailCommand(const char* args)
{
    // format: name "subject text" "mail text"
    Player* target;
    uint64 target_guid;
    std::string target_name;
    if (!extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
        return false;

    char* tail1 = strtok(NULL, "");
    if (!tail1)
        return false;

    char* msgSubject = extractQuotedArg(tail1);
    if (!msgSubject)
        return false;

    char* tail2 = strtok(NULL, "");
    if (!tail2)
        return false;

    char* msgText = extractQuotedArg(tail2);
    if (!msgText)
        return false;

    // msgSubject, msgText isn't NUL after prev. check
    std::string subject = msgSubject;
    std::string text    = msgText;

    // from console show not existed sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

    //- TODO: Fix poor design
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    MailDraft(subject, text)
        .SendMailTo(trans, MailReceiver(target, GUID_LOPART(target_guid)), sender);

    CharacterDatabase.CommitTransaction(trans);

    std::string nameLink = playerLink(target_name);
    PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
    return true;
}

//Summon group of player
bool ChatHandler::HandleGroupSummonCommand(const char* args)
{
    Player* target;
    if (!extractPlayerTarget((char*)args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target, 0))
        return false;

    Group* grp = target->GetGroup();

    std::string nameLink = GetNameLink(target);

    if (!grp)
    {
        PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    Map* gmMap = m_session->GetPlayer()->GetMap();
    bool to_instance = gmMap->Instanceable();

    // we are in instance, and can summon only player in our group with us as lead
    if (to_instance && (
        !m_session->GetPlayer()->GetGroup() || (grp->GetLeaderGUID() != m_session->GetPlayer()->GetGUID()) ||
        (m_session->GetPlayer()->GetGroup()->GetLeaderGUID() != m_session->GetPlayer()->GetGUID())))
        // the last check is a bit excessive, but let it be, just in case
    {
        SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->getSource();

        if (!player || player == m_session->GetPlayer() || !player->GetSession())
            continue;

        // check online security
        if (HasLowerSecurity(player, 0))
            return false;

        std::string plNameLink = GetNameLink(player);

        if (player->IsBeingTeleported() == true)
        {
            PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        if (to_instance)
        {
            Map* plMap = player->GetMap();

            if (plMap->Instanceable() && plMap->GetInstanceId() != gmMap->GetInstanceId())
            {
                // cannot summon from instance to instance
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, plNameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, plNameLink.c_str(), "");
        if (needReportToTarget(player))
            ChatHandler(player).PSendSysMessage(LANG_SUMMONED_BY, GetNameLink().c_str());

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        // before GM
        float x, y, z;
        m_session->GetPlayer()->GetClosePoint(x, y, z, player->GetObjectSize());
        player->TeleportTo(m_session->GetPlayer()->GetMapId(), x, y, z, player->GetOrientation());
    }

    return true;
}
