#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "MapManager.h"

class warp_commandscript : public CommandScript
{
 public:
   warp_commandscript() : CommandScript("warp_commandscript") { }
 
   ChatCommand* GetCommands() const
   {
       static ChatCommand warpCommandTable[] =
       {
           { "",       SEC_PLAYER,  true,  &HandleWarpCommand,                                 "", NULL },
           { NULL,             0,                  false, NULL,                                "", NULL }
       };

       static ChatCommand commandTable[] =
       {
           { "transport",          SEC_PLAYER,     false, NULL,                  "", warpCommandTable   },
           { NULL,            0,                   false, NULL,                                "", NULL }
       };
       return commandTable;
      }

   static bool HandleWarpCommand(ChatHandler* handler, char const* args)
   {
       if (!*args)
           return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* arg1 = strtok((char*)args, " "); // first argument is left, right, front, back, up, down, orentation
        char* arg2 = strtok(NULL, " "); // second argument is value of transport
        char* arg3 = strtok(NULL, " "); // third argument is z, if z is placed, then transport in current z level. not map z level

        if (!arg1 || !arg2 || !arg3)
        return false;

        char dir = arg1[0];
        char noz = arg3[0];
        float value = float(atof(arg2));
        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z;
        float o = player->GetOrientation();
        uint32 mapid = player->GetMapId();
        Map const* map = sMapMgr->CreateBaseMap(mapid);
        
        switch (noz)
        {
            case 'z': // z means use waterlevel for map z
            {
                z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
            }
            break;
            case 'nz': // nz means use current location z
            { 
                z = player->GetPositionZ();
            }
            break;
        }

        switch (dir)
        {
        case 'l':  // .transport left nz
        {
            x = x + cos(o+(M_PI/2))*value;
            y = y + sin(o+(M_PI/2))*value;
            player->TeleportTo(mapid, x, y, z, o); 
        }
        break;
        case 'r':  // .transport right nz
        {
            x = x + cos(o-(M_PI/2))*value;
            y = y + sin(o-(M_PI/2))*value;
            player->TeleportTo(mapid, x, y, z, o);
        }
        break;
        case 'f': // .transport front nz
        {
            x = x + cosf(o)*value;
            y = y + sinf(o)*value;
            player->TeleportTo(mapid, x, y, z, o);
        }
        break;
        case 'b':  // .transport back nz
        {
            x = x - cosf(o)*value;
            y = y - sinf(o)*value;
            player->TeleportTo(mapid, x, y, z, o);
        }
        break;
        case 'u':  // .transport up nz
        {
            player->TeleportTo(mapid, x, y, z + value, o);
        }
        break;
        case 'd':  // .transport down nz
        {
            player->TeleportTo(mapid, x, y, z - value, o);
        }
        break;
        case 'o':  // .transport orientation [Degree of Orientation] nz
        {
            o = MapManager::NormalizeOrientation((value * M_PI_F/180.0f)+ o);
            player->TeleportTo(mapid, x, y, z, o);
        }
        break;
        }
    return true;
  }

};

void AddSC_warp_commandscript()
{
    new warp_commandscript();
}