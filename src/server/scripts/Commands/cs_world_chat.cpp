#include "ScriptPCH.h"
#include "Chat.h"

class cs_world_chat : public CommandScript
{
	public:
		cs_world_chat() : CommandScript("cs_world_chat"){}

	ChatCommand * GetCommands() const
	{
		static ChatCommand WorldChatCommandTable[] = 
		{
			{"chat",	SEC_PLAYER,		true,		&HandleWorldChatCommand,	"", NULL},
			{NULL,		0,				false,		NULL,						"", NULL}
		};

		return WorldChatCommandTable;
	}

	static bool HandleWorldChatCommand(ChatHandler * handler, const char * args)
	{
		std::string msg = "";
		Player * player = handler->GetSession()->GetPlayer();

			if (player->GetSession()->GetSecurity() == 1)
			{
				msg += "|cff00ffff[VIP] |cff00ffff[";
				msg += player->GetName();
				msg += "] |cff00ccff";
			}
			if (player->GetSession()->GetSecurity() >= 2)
			{
				msg += "|cff00ffff[Staff] |cff00ffff[";
				msg += player->GetName();
				msg += "] |cff00ccff";
			}
		else
		{
			if (player->GetTeam() == ALLIANCE)
			{
				msg += "|cff0000ff[A] |cffffffff[";
				msg += player->GetName();
				msg += "] |cff00ccff";
			}

			if (player->GetTeam() == HORDE)
			{
				msg += "|cffff0000[H] |cffffffff[";
				msg += player->GetName();
				msg += "] |cff00ccff";
			}
		}
			
		msg += args;
		sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str(), 0);	

		return true;
	}
};

void AddSC_cs_world_chat()
{
	new cs_world_chat();
}