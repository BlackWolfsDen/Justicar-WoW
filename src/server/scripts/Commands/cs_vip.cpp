#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"

#define MSG_COLOR_LIGHTRED      "|cffff6060"
#define MSG_COLOR_LIGHTBLUE     "|cff00ccff"
#define MSG_COLOR_BLUE          "|cff0000ff"
#define MSG_COLOR_GREEN         "|cff00ff00"
#define MSG_COLOR_RED           "|cffff0000"
#define MSG_COLOR_GOLD          "|cffffcc00"
#define MSG_COLOR_GREY          "|cff888888"
#define MSG_COLOR_WHITE         "|cffffffff"
#define MSG_COLOR_SUBWHITE      "|cffbbbbbb"
#define MSG_COLOR_MAGENTA       "|cffff00ff"
#define MSG_COLOR_YELLOW        "|cffffff00"
#define MSG_COLOR_CYAN          "|cff00ffff"
#define MSG_COLOR_ORANGE	    "|cffff9900"

/*
* Messeges to be used in the morph commands
*/
#define VIP_MORPH_NO_NAME "Please enter a name to search for!"
#define VIP_TAG_NO_NAME "Please enter a name to search for!"
#define VIP_MORPH_NO_SUCH_NAME "There is no such morph name!"
#define VIP_TAG_NO_SUCH_NAME "There is no such morph name!"
#define VIP_MORPH_NO_SUCH_DISPLAY_ID "There is no such display!"
#define VIP_MORPH_NOT_ALLOWED "You are not allowed to use this command!"
#define VIP_MORPH_TOO_MANY_RESULTS "There were too many results, please be more specific!"
#define VIP_TAG_TOO_MANY_RESULTS "There were too many results, please be more specific!"

#define OFFSET_THEME 10000 

enum VIP_MORPH
{
	VIP_MORPH_MAX_RESULTS = 25,
};

class vip_commandscript : public CommandScript
{
public:
	vip_commandscript() : CommandScript("vip_commandscript") { }

	ChatCommand* GetCommands() const
	{
		static ChatCommand TAGCommandTable[] =
		{
			{ "lookup",         SEC_PLAYER,       false, &HandleTAGLookupCommand,            "", NULL },
			{ "dp",             SEC_PLAYER,       false, &HandleTAGDPTagCommand,             "", NULL },
			{ "vp",             SEC_PLAYER,       false, &HandleTAGVPTagCommand,             "", NULL },
			{ "ec", 	      SEC_PLAYER, 	   false, &HandleTAGECTagCommand, 		    "", NULL },
			{ NULL,             0,                false,  NULL,              		    "", NULL }
		};
		static ChatCommand VIPMorphCommandTable[] =
		{
			{ "lookup",         SEC_PLAYER,        false, &HandleVIPMorphListCommand,            "", NULL },
			{ "",               SEC_PLAYER,        false, &HandleVIPMorphCommand,                "", NULL },
			{ NULL,             0,                 false, NULL,                                  "", NULL }
		};
		static ChatCommand VIPHonorCommandTable[] =
		{
			{ "add",             SEC_MODERATOR,     false, &HandleVIPHonorAddCommand,             "", NULL },
			{ NULL,              0,                  false, NULL,                                  "", NULL }
		};
		static ChatCommand VIPTitlesCommandTable[] =
		{
			{ "add",             SEC_MODERATOR,     false, &HandleVIPTitlesAddCommand,            "", NULL },
			{ "remove",          SEC_MODERATOR,     false, &HandleVIPTitlesRemoveCommand,         "", NULL },
			{ NULL,              0,                  false, NULL,                                  "", NULL }
		};
		static ChatCommand VIPjoinCommandTable[] =
		{
			{ "arena",          SEC_MODERATOR,     false,  &HandleVIPjoinArenaCommand,           "", NULL },
			{ "warsong",        SEC_MODERATOR,     false,  &HandleVIPjoinWarsongCommand,         "", NULL },
			{ "arathi",         SEC_MODERATOR,     false,  &HandleVIPjoinArathiCommand,          "", NULL },
			{ "eye",            SEC_MODERATOR,     false,  &HandleVIPjoinEyeCommand,             "", NULL },
			{ "alterac",        SEC_MODERATOR,     false,  &HandleVIPjoinAlteracCommand,         "", NULL },
			{ NULL,              0,                  false,  NULL,                                 "", NULL }
		};
		static ChatCommand VIPCommandTable[] =
		{
			{ "demorph",      SEC_MODERATOR,     false,   &HandleVIPDeMorphCommand, 		"", NULL },
			{ "extra",	      SEC_MODERATOR,     false,   &HandleVIPextraCommand,             "", NULL },
			{ "revive",       SEC_MODERATOR,     false,   &HandleVIPreviveCommand,             "", NULL },
			{ "heal",         SEC_MODERATOR,     false,   &HandleVIPhealCommand,               "", NULL },
			{ "buffs",        SEC_MODERATOR,     false,   &HandleVIPbuffsCommand,              "", NULL },
			{ "mall",         SEC_MODERATOR,     false,   &HandleVIPmallCommand,               "", NULL },
			{ "oldmall",      SEC_MODERATOR,     false,   &HandleVIPoldmallCommand,            "", NULL },
			{ "gear",         SEC_MODERATOR,     false,   &HandleVIPgearCommand,               "", NULL },
			{ "announce",     SEC_MODERATOR,     false,   &HandleVIPannounceCommand,           "", NULL },
			{ "chat",         SEC_MODERATOR,     false,   &HandleVIPchatCommand,               "", NULL },
			{ "combatstop",   SEC_MODERATOR,     false,   &HandleVIPCombatStopCommand,         "", NULL },
			{ "gamble",       SEC_MODERATOR,     false,   &HandleVIPGambleCommand,             "", NULL },
			{ "roulette",     SEC_MODERATOR,     false,   &HandleVIPRouletteCommand,           "", NULL },
			{ "scale",        SEC_MODERATOR,     false,   &HandleVIPScaleCommand,              "", NULL },
			{ "respawn",      SEC_MODERATOR,     false,   &HandleVIPRespawnCommand,            "", NULL },
			{ "waterwalk",    SEC_MODERATOR,     false,   &HandleVIPWaterwalkCommand,          "", NULL },
			{ "changerace",   SEC_MODERATOR,     false,   &HandleVIPFreeChangeRaceCommand,     "", NULL },
			{ "online",       SEC_MODERATOR,     false,   &HandleVIPOnlineCommand,             "", NULL },
			{ "join",         SEC_MODERATOR,     false,  NULL,              "", VIPjoinCommandTable },
			{ "title",        SEC_MODERATOR,     false,  NULL,              "", VIPTitlesCommandTable },
			{ "honor",        SEC_MODERATOR,     false,  NULL,              "", VIPHonorCommandTable },
			{ "morph",        SEC_MODERATOR,     false,  NULL,              "", VIPMorphCommandTable },
			{ NULL,             0,                false,  NULL,              "", NULL }
		};

		static ChatCommand commandTable[] =
		{
			{ "tag",	      SEC_PLAYER, 	    false, NULL, 		   "",  TAGCommandTable},
			{ "vip",             SEC_MODERATOR,     false, NULL,               "",  VIPCommandTable},
			{ NULL,              0,                  false, NULL,                               "", NULL }
		};

		return commandTable;
	}

	static bool HandleVIPDeMorphCommand(ChatHandler* handler, const char* /*args*/)
	{
		Player* player = handler->GetSession()->GetPlayer();

		player->DeMorph();

		return true;
	}

	static bool HandleVIPMorphCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;
		Player* pPlayer = handler->GetSession()->GetPlayer();

		if (!*args)
		{
			handler->SendSysMessage(VIP_MORPH_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if(handler->GetSession()->GetSecurity() != 0){
			std::string name = args;
			std::ostringstream msg;
			if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `displayId`,`displayName` FROM `custom_vip_morph` WHERE `displayName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
			{
				uint32 displayId = queryResult->Fetch()[0].GetUInt32();
				std::string displayName = queryResult->Fetch()[1].GetString();

				pPlayer->SetDisplayId(displayId);
				msg << MSG_COLOR_SUBWHITE << "Your new display is: " << MSG_COLOR_LIGHTBLUE  << displayName.c_str( ) << MSG_COLOR_SUBWHITE << ".";
				ChatHandler(pPlayer).SendSysMessage(msg.str( ).c_str( ));
			}else{
				handler->SendSysMessage(VIP_MORPH_NO_SUCH_NAME);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}else{
			handler->SendSysMessage(VIP_MORPH_NO_SUCH_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}
		return true;
	}

	static bool HandleVIPMorphListCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;
		Player* pPlayer = handler->GetSession()->GetPlayer();
		if (!*args)
		{
			handler->SendSysMessage(VIP_MORPH_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string name = args;
		uint32 count = 0;
		uint32 maxResults = VIP_MORPH_MAX_RESULTS;

		bool limitReached = false;
		if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `displayId`,`displayName` FROM `custom_vip_morph` WHERE `displayName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
		{
			do
			{
				std::ostringstream msg;
				std::ostringstream displayID;
				std::ostringstream displayNAME;
				uint32 displayId = queryResult->Fetch()[0].GetUInt32();
				std::string displayName = queryResult->Fetch()[1].GetString();

				displayID << displayId;
				displayNAME << displayName;

				msg << MSG_COLOR_SUBWHITE << "Name: [" << MSG_COLOR_LIGHTBLUE  << displayName.c_str() << MSG_COLOR_SUBWHITE << "] Display ID: " << MSG_COLOR_LIGHTBLUE  << displayID.str() << MSG_COLOR_SUBWHITE << ". \n";
				ChatHandler(pPlayer).SendSysMessage(msg.str().c_str());

				if (maxResults && count++ == maxResults)
				{
					handler->PSendSysMessage(VIP_MORPH_TOO_MANY_RESULTS, maxResults);
					limitReached = true;
				}
			}while(queryResult->NextRow() && limitReached == false);
		}else{
			handler->SendSysMessage(VIP_MORPH_NO_SUCH_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}

		return true;
	}

	static bool HandleTAGDPTagCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;

		Player* pPlayer = handler->GetSession()->GetPlayer();

		if (!*args)
		{
			handler->SendSysMessage(VIP_TAG_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string name = args;
		std::ostringstream msg;

		if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `tagId`,`tagName`,`dp` FROM `custom_tag` WHERE `tagName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
		{
			uint32 tagId = queryResult->Fetch()[0].GetUInt32();
			std::string tagName = queryResult->Fetch()[1].GetString();
			uint32 dp = queryResult->Fetch()[2].GetUInt32();

			if(pPlayer->GetDonationPoints(pPlayer->GetSession()->GetAccountId()) >= dp)
			{
				pPlayer->AddAura(tagId, pPlayer);
				LoginDatabase.PExecute("UPDATE fusion.account_data SET dp = dp - '%u' WHERE id = '%u'", dp, pPlayer->GetSession()->GetAccountId());
				msg << MSG_COLOR_SUBWHITE << "Your new tag is: " << MSG_COLOR_LIGHTBLUE  << tagName.c_str( ) << MSG_COLOR_SUBWHITE << ".";
				ChatHandler(pPlayer).SendSysMessage(msg.str( ).c_str( ));
			}
			else
			{
				handler->PSendSysMessage("You do not have enough DP for this tag. Price is: %u", dp);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}
		else 
		{
			handler->PSendSysMessage("There is no tag with this name.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		return true;
	}

	static bool HandleTAGVPTagCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;

		Player* pPlayer = handler->GetSession()->GetPlayer();

		if (!*args)
		{
			handler->SendSysMessage(VIP_TAG_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string name = args;
		std::ostringstream msg;

		if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `tagId`,`tagName`,`vp` FROM `custom_tag` WHERE `tagName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
		{
			uint32 tagId = queryResult->Fetch()[0].GetUInt32();
			std::string tagName = queryResult->Fetch()[1].GetString();
			uint32 vp = queryResult->Fetch()[2].GetUInt32();

			if(pPlayer->GetVotingPoints(pPlayer->GetSession()->GetAccountId()) >= vp)
			{
				pPlayer->AddAura(tagId, pPlayer);
				LoginDatabase.PExecute("UPDATE fusion.account_data SET vp = vp - '%u' WHERE id = '%u'", vp, pPlayer->GetSession()->GetAccountId());
				msg << MSG_COLOR_SUBWHITE << "Your new tag is: " << MSG_COLOR_LIGHTBLUE  << tagName.c_str( ) << MSG_COLOR_SUBWHITE << ".";
				ChatHandler(pPlayer).SendSysMessage(msg.str( ).c_str( ));
			}
			else
			{
				handler->PSendSysMessage("You do not have enough VP for this tag. Price is: %u", vp);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}
		else 
		{
			handler->PSendSysMessage("There is no tag with this name.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		return true;
	}

	static bool HandleTAGECTagCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;

		Player* pPlayer = handler->GetSession()->GetPlayer();

		if (!*args)
		{
			handler->SendSysMessage(VIP_TAG_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string name = args;
		std::ostringstream msg;

		if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `tagId`,`tagName`,`ec` FROM `custom_tag` WHERE `tagName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
		{
			uint32 tagId = queryResult->Fetch()[0].GetUInt32();
			std::string tagName = queryResult->Fetch()[1].GetString();
			uint32 ec = queryResult->Fetch()[2].GetUInt32();

			if(pPlayer->HasItemCount(23247, ec))
			{
				pPlayer->AddAura(tagId, pPlayer);
				pPlayer->DestroyItemCount(23247, ec, true);
				msg << MSG_COLOR_SUBWHITE << "Your new tag is: " << MSG_COLOR_LIGHTBLUE  << tagName.c_str( ) << MSG_COLOR_SUBWHITE << ".";
				ChatHandler(pPlayer).SendSysMessage(msg.str( ).c_str( ));
			}
			else
			{
				handler->PSendSysMessage("You do not have enough Event Coins for this tag. Price is: %u", ec);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}
		else 
		{
			handler->PSendSysMessage("There is no tag with this name.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		return true;
	}

	static bool HandleTAGLookupCommand(ChatHandler* handler, char const* args)
	{
		if (!handler)
			return false;
		Player* pPlayer = handler->GetSession()->GetPlayer();
		if (!*args)
		{
			handler->SendSysMessage(VIP_MORPH_NO_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (pPlayer->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string name = args;
		uint32 count = 0;
		uint32 maxResults = VIP_MORPH_MAX_RESULTS;

		bool limitReached = false;
		if(QueryResult queryResult = CharacterDatabase.PQuery("SELECT `tagName`,`dp`,`vp`,`ec` FROM `custom_tag` WHERE `tagName` "_LIKE_" "_CONCAT3_("'%%'","'%s'","'%%'"), name.c_str()))
		{
			do
			{
				std::ostringstream msg;
				std::ostringstream tagNAME;
				std::string tagName = queryResult->Fetch()[0].GetString();
				uint32 dp = queryResult->Fetch()[1].GetUInt32();
				uint32 vp = queryResult->Fetch()[2].GetUInt32();
				uint32 ec = queryResult->Fetch()[3].GetUInt32();

				tagNAME << tagName;
				dp << dp;
				vp << vp;
				ec << ec;

				msg << MSG_COLOR_SUBWHITE << "Tag name: [" << MSG_COLOR_LIGHTBLUE  << tagName.c_str() << MSG_COLOR_SUBWHITE << "] Price DP: " << MSG_COLOR_LIGHTBLUE << dp << MSG_COLOR_SUBWHITE << " Price VP: " << MSG_COLOR_LIGHTBLUE  << vp << MSG_COLOR_SUBWHITE << " Price EC: " << MSG_COLOR_LIGHTBLUE  << ec << MSG_COLOR_SUBWHITE << ". \n";
				ChatHandler(pPlayer).SendSysMessage(msg.str().c_str());

				if (maxResults && count++ == maxResults)
				{
					handler->PSendSysMessage(VIP_TAG_TOO_MANY_RESULTS, maxResults);
					limitReached = true;
				}
			}while(queryResult->NextRow() && limitReached == false);
		}else{
			handler->SendSysMessage(VIP_TAG_NO_SUCH_NAME);
			handler->SetSentErrorMessage(true);
			return false;
		}

		return true;
	}

	static bool HandleVIPextraCommand(ChatHandler* handler, const char* /*args*/)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if(player->HasSpell(42365) || player->HasSpell(58983) || player->HasSpell(61855))
		{
			handler->PSendSysMessage("|cFF00CC00You already have VIP perks!|r");
			handler->SetSentErrorMessage(true);
			return false;
		}

		player->learnSpell(42365, false);
		player->learnSpell(58983, false);
		player->learnSpell(61855, false);
		handler->PSendSysMessage("|cFF00CC00Enjoy your extra VIP perks!|r");
		return true;
	}

	static bool HandleVIPOnlineCommand(ChatHandler* handler, const char* /*args*/)
	{
		bool first = true;
		bool footer = false;

		ACE_READ_GUARD_RETURN(HashMapHolder<Player>::LockType, guard, *HashMapHolder<Player>::GetLock(), true);
		HashMapHolder<Player>::MapType const& m = sObjectAccessor->GetPlayers();
		for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
		{
			AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
			if ((itr_sec == SEC_MODERATOR && itr_sec <= AccountTypes(sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_GM_LIST))) &&
				(!handler->GetSession() || itr->second->IsVisibleGloballyFor(handler->GetSession()->GetPlayer())))
			{
				if (first)
				{
					first = false;
					footer = true;
					handler->SendSysMessage(LANG_VIPS_ON_SRV);
					handler->SendSysMessage("========================");
				}
				const char* name = itr->second->GetName();
				uint8 security = itr_sec;
				uint8 max = ((16 - strlen(name)) / 2);
				uint8 max2 = max;
				if (((max)+(max2)+(strlen(name))) == 16)
					max2 = ((max)-1);
				if (handler->GetSession())
					handler->PSendSysMessage("|    %s", name);
				else
					handler->PSendSysMessage("|%*s%s%*s|   %u  |", max, " ", name, max2, " ", security);
			}
		}
		if (footer)
			handler->SendSysMessage("========================");
		if (first)
			handler->SendSysMessage(LANG_VIPS_NOT_LOGGED);
		return true;
	}

	static bool HandleVIPHonorAddCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player *target = handler->GetSession()->GetPlayer();

		// check online security
		if (handler->HasLowerSecurity(target, 0))
			return false;

		uint32 amount = (uint32)atoi(args);
		if (amount > 100000)
		{
			handler->PSendSysMessage(LANG_BAD_VALUE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->RewardHonor(NULL, 1, amount);
		return true;
	}

	static bool HandleVIPTitlesAddCommand(ChatHandler* handler, const char* args)
	{
		// number or [name] Shift-click form |color|Htitle:title_id|h[name]|h|r
		char* id_p = handler->extractKeyFromLink((char*)args, "Htitle");
		if (!id_p)
			return false;

		int32 id = atoi(id_p);
		if (id <= 0)
		{
			handler->PSendSysMessage(LANG_INVALID_TITLE_ID, id);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Player* target = handler->GetSession()->GetPlayer();

		// check online security
		if (handler->HasLowerSecurity(target, 0))
			return false;

		CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
		if (!titleInfo)
		{
			handler->PSendSysMessage(LANG_INVALID_TITLE_ID, id);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string tNameLink = handler->GetNameLink(target);

		char const* targetName = target->GetName();
		char titleNameStr[80];
		snprintf(titleNameStr, 80, titleInfo->name[handler->GetSessionDbcLocale()], targetName);

		target->SetTitle(titleInfo);
		handler->PSendSysMessage(LANG_TITLE_ADD_RES, id, titleNameStr, tNameLink.c_str());

		return true;
	}

	static bool HandleVIPTitlesRemoveCommand(ChatHandler* handler, const char* args)
	{
		// number or [name] Shift-click form |color|Htitle:title_id|h[name]|h|r
		char* id_p = handler->extractKeyFromLink((char*)args, "Htitle");
		if (!id_p)
			return false;

		int32 id = atoi(id_p);
		if (id <= 0)
		{
			handler->PSendSysMessage(LANG_INVALID_TITLE_ID, id);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Player* target = handler->GetSession()->GetPlayer();

		// check online security
		if (handler->HasLowerSecurity(target, 0))
			return false;

		CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(id);
		if (!titleInfo)
		{
			handler->PSendSysMessage(LANG_INVALID_TITLE_ID, id);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->SetTitle(titleInfo, true);

		std::string tNameLink = handler->GetNameLink(target);

		char const* targetName = target->GetName();
		char titleNameStr[80];
		snprintf(titleNameStr, 80, titleInfo->name[handler->GetSessionDbcLocale()], targetName);

		handler->PSendSysMessage(LANG_TITLE_REMOVE_RES, id, titleNameStr, tNameLink.c_str());

		if (!target->HasTitle(target->GetInt32Value(PLAYER_CHOSEN_TITLE)))
		{
			target->SetUInt32Value(PLAYER_CHOSEN_TITLE, 0);
			handler->PSendSysMessage(LANG_CURRENT_TITLE_RESET, tNameLink.c_str());
		}

		return true;
	}

	static bool HandleVIPRespawnCommand(ChatHandler* handler, const char* /*args*/)
	{
		Player* pl = handler->GetSession()->GetPlayer();

		if (pl->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (pl->GetAreaId() == 856)
		{
			handler->PSendSysMessage("You can't respawn the Event Boss.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (pl->GetAreaId() == 255)
		{
			handler->PSendSysMessage("Emeth forbids you to use that command here.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (pl->GetMapId() == 532 || pl->GetMapId() == 631)
		{
			handler->PSendSysMessage("The will of dead is too strong in here..");
			handler->SetSentErrorMessage(true);
			return false;
		}

		// accept only explicitly selected target (not implicitly self targeting case)
		Unit* target = handler->getSelectedUnit();

		if (!target || target->GetTypeId() != TYPEID_UNIT || target->isPet() || target->GetEntry() == 986507)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (pl->GetSelection() && target)
		{
			if (target->isAlive())
				handler->PSendSysMessage("You can only revive dead creatures!");
			else
				target->ToCreature()->Respawn();
			return true;
		}

		return true;
	}

	static bool HandleVIPjoinArathiCommand(ChatHandler* handler, const char* /*args*/)
	{
		BattlegroundTypeId bgTypeId = BATTLEGROUND_AB;
		handler->GetSession()->SendBattleGroundList(handler->GetSession()->GetPlayer()->GetGUID(), bgTypeId);
		handler->PSendSysMessage(LANG_QUEUE_ARATHI);
		return true;
	}

	static bool HandleVIPjoinEyeCommand(ChatHandler* handler, const char* /*args*/)
	{
		BattlegroundTypeId bgTypeId = BATTLEGROUND_EY;
		handler->GetSession()->SendBattleGroundList(handler->GetSession()->GetPlayer()->GetGUID(), bgTypeId);
		handler->PSendSysMessage(LANG_QUEUE_EYE);
		return true;
	}

	static bool HandleVIPjoinWarsongCommand(ChatHandler* handler, const char* /*args*/)
	{
		BattlegroundTypeId bgTypeId = BATTLEGROUND_WS;
		handler->GetSession()->SendBattleGroundList(handler->GetSession()->GetPlayer()->GetGUID(), bgTypeId);
		handler->PSendSysMessage(LANG_QUEUE_WARSONG);
		return true;
	}

	static bool HandleVIPjoinAlteracCommand(ChatHandler* handler, const char* /*args*/)
	{
		BattlegroundTypeId bgTypeId = BATTLEGROUND_AV;
		handler->GetSession()->SendBattleGroundList(handler->GetSession()->GetPlayer()->GetGUID(), bgTypeId);
		handler->PSendSysMessage(LANG_QUEUE_ALTERAC);
		return true;
	}

	static bool HandleVIPjoinArenaCommand(ChatHandler* handler, const char* /*args*/)
	{
		BattlegroundTypeId bgTypeId = BATTLEGROUND_AA;
		handler->GetSession()->SendBattleGroundList(handler->GetSession()->GetPlayer()->GetGUID(), bgTypeId);
		handler->PSendSysMessage(LANG_QUEUE_ARENA);
		return true;
	}

	static bool HandleVIPWaterwalkCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player *player = handler->GetSession()->GetPlayer();

		if (player->GetAreaId() == 2408)
		{
			handler->PSendSysMessage(LANG_NOT_IN_PVPMALL);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (strncmp(args, "on", 3) == 0)
			player->SetMovement(MOVE_WATER_WALK);               // ON
		else if (strncmp(args, "off", 4) == 0)
			player->SetMovement(MOVE_LAND_WALK);                // OFF
		else
		{
			handler->PSendSysMessage(LANG_USE_BOL);
			return false;
		}


		handler->PSendSysMessage(LANG_YOU_SET_WATERWALK, args, handler->GetNameLink(player).c_str());
		if (handler->needReportToTarget(player))
			handler->PSendSysMessage(LANG_YOUR_WATERWALK_SET, args, handler->GetNameLink().c_str());
		return true;
	}

	static bool HandleVIPScaleCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		float Scale = (float)atof((char*)args);
		if (Scale > 1.5f || Scale < 0.5f)
		{
			handler->PSendSysMessage(LANG_BAD_VALUE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Player* target = handler->GetSession()->GetPlayer();
		if (target->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (target->getRace() == RACE_VRYKUL)
		{
			handler->PSendSysMessage("Vrykuls do not have the power to modify their scale.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		handler->PSendSysMessage(LANG_YOU_CHANGE_SIZE, Scale, handler->GetNameLink(target).c_str());
		if (handler->needReportToTarget(target))
			handler->PSendSysMessage(LANG_YOURS_SIZE_CHANGED, handler->GetNameLink().c_str(), Scale);

		target->RemoveAurasDueToSpell(16591);
		target->RemoveAurasDueToSpell(16593);
		target->RemoveAurasDueToSpell(16595);
		target->SetFloatValue(OBJECT_FIELD_SCALE_X, Scale);

		return true;
	}

	static bool HandleVIPgearCommand(ChatHandler* handler, const char* args)
	{

		Player *pPlr = handler->GetSession()->GetPlayer();
		ItemPosCountVec dest;
		Item* pItem;

		switch(pPlr->getClass())
		{

		case CLASS_WARRIOR:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200265, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200265,true,Item::GenerateItemRandomPropertyId(200265));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200266, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200266,true,Item::GenerateItemRandomPropertyId(200266));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200267, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200267,true,Item::GenerateItemRandomPropertyId(200267));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200268, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200268,true,Item::GenerateItemRandomPropertyId(200268));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200269, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200269,true,Item::GenerateItemRandomPropertyId(200269));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			break;

		case CLASS_PALADIN:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200270, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200270,true,Item::GenerateItemRandomPropertyId(200270));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200271, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200271,true,Item::GenerateItemRandomPropertyId(200271));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200272, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200272,true,Item::GenerateItemRandomPropertyId(200272));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200273, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200273,true,Item::GenerateItemRandomPropertyId(200273));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200274, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200274,true,Item::GenerateItemRandomPropertyId(200274));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!
			break;
		case CLASS_HUNTER:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200250, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200250,true,Item::GenerateItemRandomPropertyId(200250));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200251, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200251,true,Item::GenerateItemRandomPropertyId(200251));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200252, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200252,true,Item::GenerateItemRandomPropertyId(200252));
			pItem = pPlr->StoreNewItem(dest, 200252,true,Item::GenerateItemRandomPropertyId(200252));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200253, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200253,true,Item::GenerateItemRandomPropertyId(200253));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200254, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200254,true,Item::GenerateItemRandomPropertyId(200254));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_ROGUE:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200260, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200260,true,Item::GenerateItemRandomPropertyId(200260));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200261, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200261,true,Item::GenerateItemRandomPropertyId(200261));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200262, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200262,true,Item::GenerateItemRandomPropertyId(200262));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200263, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200263,true,Item::GenerateItemRandomPropertyId(200263));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200264, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200264,true,Item::GenerateItemRandomPropertyId(200264));
			pItem = pPlr->StoreNewItem(dest, 200264,true,Item::GenerateItemRandomPropertyId(200264));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_PRIEST:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200241, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200241,true,Item::GenerateItemRandomPropertyId(200241));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200242, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200242,true,Item::GenerateItemRandomPropertyId(200242));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200243, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200243,true,Item::GenerateItemRandomPropertyId(200243));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200244, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200244,true,Item::GenerateItemRandomPropertyId(200244));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200521, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200521,true,Item::GenerateItemRandomPropertyId(200521));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_DEATH_KNIGHT:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200275, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200275,true,Item::GenerateItemRandomPropertyId(200275));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200276, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200276,true,Item::GenerateItemRandomPropertyId(200276));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200277, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200277,true,Item::GenerateItemRandomPropertyId(200277));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200278, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200278,true,Item::GenerateItemRandomPropertyId(200278));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200279, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200279,true,Item::GenerateItemRandomPropertyId(200279));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_SHAMAN:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200245, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200245,true,Item::GenerateItemRandomPropertyId(200245));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200246, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200246,true,Item::GenerateItemRandomPropertyId(200246));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200247, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200247,true,Item::GenerateItemRandomPropertyId(200247));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200248, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200248,true,Item::GenerateItemRandomPropertyId(200248));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200249, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200249,true,Item::GenerateItemRandomPropertyId(200249));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_MAGE:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200236, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200236,true,Item::GenerateItemRandomPropertyId(200236));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200237, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200237,true,Item::GenerateItemRandomPropertyId(200237));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200238, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200238,true,Item::GenerateItemRandomPropertyId(200238));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200239, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200239,true,Item::GenerateItemRandomPropertyId(200239));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200240, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200240,true,Item::GenerateItemRandomPropertyId(200240));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_WARLOCK:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200231, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200231,true,Item::GenerateItemRandomPropertyId(200231));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200232, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200232,true,Item::GenerateItemRandomPropertyId(200232));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200233, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200233,true,Item::GenerateItemRandomPropertyId(200233));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200234, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200234,true,Item::GenerateItemRandomPropertyId(200234));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200235, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200235,true,Item::GenerateItemRandomPropertyId(200235));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;
		case CLASS_DRUID:
			//send 1 item
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200255, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200255,true,Item::GenerateItemRandomPropertyId(200255));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 2 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200256, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200256,true,Item::GenerateItemRandomPropertyId(200256));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 3 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200257, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200257,true,Item::GenerateItemRandomPropertyId(200257));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 4 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200258, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200258,true,Item::GenerateItemRandomPropertyId(200258));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!

			//send 5 items
			pPlr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 200259, 1, 0);
			pItem = pPlr->StoreNewItem(dest, 200259,true,Item::GenerateItemRandomPropertyId(200259));
			pPlr->SendNewItem(pItem, 1, true, false);
			dest.clear(); //anytime you add another item. you have to add this to the end! dont forget!;
			break;

		default: break;

		}

		return true;

	}

	static bool HandleVIPchatCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		WorldPacket data;
		if (!*args)
			return false;

		sWorld->SendVIPText(LANG_VIP_CHAT, handler->GetNameLink(player).c_str(), args);

		return true;
	}

	static bool HandleVIPannounceCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		WorldPacket data;
		if (!*args)
			return false;

		sWorld->SendVIPText(LANG_VIP_NAME_ANNOUNCE, handler->GetNameLink(player).c_str(), args);

		return true;
	}

	static bool HandleVIPFreeChangeRaceCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->GetSession()->GetPlayer();

		if(target->getLevel() == 255)
		{
			handler->PSendSysMessage(LANG_CUSTOMIZE_PLAYER, handler->GetNameLink(target).c_str());
			target->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
			CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = %u", target->GetGUIDLow());
		} else {
			handler->PSendSysMessage(LANG_MUST_BE_LEVEL_255);
		}

		return true;
	}

	static bool HandleVIPreviveCommand(ChatHandler* handler, const char* args)
	{
		Player *player = handler->GetSession()->GetPlayer();

		if (player->GetAreaId() == 2408 || player->GetAreaId() == 298 || player->GetAreaId() == 1637 || player->GetAreaId() == 1519 || player->GetAreaId() == 2266 || player->GetAreaId() == 3457)
		{
			handler->PSendSysMessage(LANG_NOT_IN_PVPMALL);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetAreaId() == 255)
		{
			handler->PSendSysMessage("Emeth forbids you to use that command here.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetMapId() == 573 || player->GetMapId() == 631)
		{
			handler->PSendSysMessage("You can't use that command here.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetAreaId() == 856)
		{
			handler->PSendSysMessage("You can't revive the Event Boss.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isAlive())
		{
			handler->PSendSysMessage(LANG_YOU_MUST_BE_FUCKING_DEAD);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInFlight())
		{
			handler->PSendSysMessage(LANG_YOU_IN_FLIGHT);
			handler->SetSentErrorMessage(true);
			return false;
		}

		player->ResurrectPlayer(0.5f);

		return true;
	}
	static bool HandleVIPhealCommand(ChatHandler* handler, const char* args)
	{
		Player *player = handler->GetSession()->GetPlayer();

		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetAreaId() == 2408 || player->GetAreaId() == 298 || player->GetAreaId() == 1637 || player->GetAreaId() == 1519 || player->GetAreaId() == 2266 || player->GetAreaId() == 3457)
		{
			handler->PSendSysMessage(LANG_NOT_IN_PVPMALL);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetAreaId() == 4603)
		{
			handler->PSendSysMessage(LANG_NOT_IN_VAULT_OF_A);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetAreaId() == 255)
		{
			handler->PSendSysMessage("Emeth forbids you to use that command here.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetMapId() == 573 || player->GetMapId() == 631)
		{
			handler->PSendSysMessage("You can't use that command here.");
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInCombat())
		{
			handler->PSendSysMessage(LANG_NOT_WHILE_FIGHTING);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInFlight())
		{
			handler->PSendSysMessage(LANG_NOT_WHILE_FLYING);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (!player->isAlive())
		{
			handler->PSendSysMessage("You can't use this command while your dead.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		{
			player->Dismount();
			player->RemoveAurasByType(SPELL_AURA_MOUNTED);
			player->SetHealth(player->GetMaxHealth());
			player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
		}

		return true;
	}
	static bool HandleVIPmallCommand(ChatHandler* handler, const char* args)
	{
		//MALL command

		Player *player = handler->GetSession()->GetPlayer();

		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInCombat())
		{
			handler->PSendSysMessage(LANG_YOU_IN_COMBAT);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInFlight())
		{
			handler->PSendSysMessage(LANG_YOU_IN_FLIGHT);
			handler->SetSentErrorMessage(true);
			return false;
		}

		switch(player->GetTeam())
		{
		case ALLIANCE:
			player->TeleportTo(1, 16237.093750f, 16260.650391f, 36.772217f, 4.699230f);    // Insert Ally mall Cords here
			break;

		case HORDE:
			player->TeleportTo(1, 16237.093750f, 16260.650391f, 36.772217f, 4.699230f);    // Insert Horde mall Cords here
			break;
		}
		return true;
	}

	static bool HandleVIPoldmallCommand(ChatHandler* handler, const char* args)
	{
		//MALL command

		Player *player = handler->GetSession()->GetPlayer();

		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInCombat())
		{
			handler->PSendSysMessage(LANG_YOU_IN_COMBAT);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInFlight())
		{
			handler->PSendSysMessage(LANG_YOU_IN_FLIGHT);
			handler->SetSentErrorMessage(true);
			return false;
		}

		switch(player->GetTeam())
		{
		case ALLIANCE:
			player->TeleportTo(568, 119.865295f, 661.080505f, 51.690681f, 1.602217f);    // Insert Ally mall Cords here
			break;

		case HORDE:
			player->TeleportTo(568, 119.865295f, 661.080505f, 51.690681f, 1.602217f);    // Insert Horde mall Cords here
			break;
		}
		return true;
	}

	static bool HandleVIPbuffsCommand(ChatHandler* handler, const char* args)
	{
		Player *player = handler->GetSession()->GetPlayer();

		if (player->isInCombat())
		{
			handler->PSendSysMessage(LANG_NOT_WHILE_FIGHTING);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->isInFlight())
		{
			handler->PSendSysMessage(LANG_NOT_WHILE_FLYING);
			handler->SetSentErrorMessage(true);
			return false;
		}
		if (player->GetMap()->IsBattleArena())
		{
			handler->PSendSysMessage("You can't use this in an arena.");
			handler->SetSentErrorMessage(true);
			return false;
		}

		{
			player->Dismount();
			player->RemoveAurasByType(SPELL_AURA_MOUNTED);
			player->AddAura(48161, player);              // Power Word: Fortitude
			player->AddAura(48073, player);              // Divine Spirit
			player->AddAura(20217, player);              // Blessing of Kings
			player->AddAura(48469, player);              // Mark of the wild
			player->AddAura(16609, player);              // Spirit of Zandalar
			player->AddAura(15366, player);              // Songflower Serenade
			player->AddAura(22888, player);              // Rallying Cry of the Dragonslayer
			player->AddAura(57399, player);              // Well Fed
			player->AddAura(17013, player);              // Agamaggan's Agility
			player->AddAura(16612, player);              // Agamaggan's Strength
			player->AddAura(24705, player);
			player->AddAura(26035, player);
			player->AddAura(31305, player);
			player->AddAura(36001, player);
			player->AddAura(70235, player);
			player->AddAura(70242, player);
			player->AddAura(70244, player);
			player->AddAura(30090, player);
			player->AddAura(30088, player);
			player->AddAura(30089, player);
		}

		return true;
	}
	//Allows your players to gamble for fun and prizes
	static bool HandleVIPGambleCommand(ChatHandler* handler, const char* args)
	{
		Player *player = handler->GetSession()->GetPlayer();

		char* px = strtok((char*)args, " ");

		if (!px)
			return false;

		uint32 money = (uint32)atoi(px);

		if (player->GetMoney() < money)
		{
			handler->PSendSysMessage("You can not bet with money you do not have!");
			return true;
		}

		else
		{
			if (money>0)
			{
				if (rand()%100 < 50)
				{
					player->ModifyMoney(money*2);
					handler->PSendSysMessage("You have won and doubled your bet");
				}
				else
				{
					player->ModifyMoney(-int(money/2));
					handler->PSendSysMessage("You have lost half of your money");
				}
			}
		}

		return true;
	}

	static bool HandleVIPRouletteCommand(ChatHandler* handler, const char* args)
	{
		Player *player = handler->GetSession()->GetPlayer();

		char* px = strtok((char*)args, " ");

		if (!px)
			return false;

		uint32 money = (uint32)atoi(px);

		if (player->GetMoney() < money)
		{
			handler->PSendSysMessage("You can not bet with money you do not have!");
			return true;
		}

		else
		{
			if (money>0)
			{
				if (rand()%36 < 1)
				{
					player->ModifyMoney(money*36);
					handler->PSendSysMessage("You have won 36 times your bet, congratulations!");
				}
				else
				{
					player->ModifyMoney(-int(money/36));
					handler->PSendSysMessage("You have lost 36 times your gold amount.");
				}
			}
		}

		return true;
	}

	static bool HandleVIPCombatStopCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (player->GetMap()->IsBattlegroundOrArena())
		{
			handler->PSendSysMessage(LANG_YOU_IN_BATTLEGROUND);
			handler->SetSentErrorMessage(true);
			return false; 
		}

		if (player->GetMap()->Instanceable())
		{
			player->CombatStop();
			player->getHostileRefManager().deleteReferences();
			handler->PSendSysMessage("you are no longer in combat!");
			return true;
		} else {
			handler->PSendSysMessage(LANG_ONLY_IN_INSTANCE);
		}
		return true;
	}
};
void AddSC_vip_commandscript()
{
	new vip_commandscript();
}
