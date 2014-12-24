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
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"

#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"

bool WorldSession::processChatmessageFurtherAfterSecurityChecks(std::string& msg, uint32 lang)
{
    if (lang != LANG_ADDON)
    {
        // strip invisible characters for non-addon messages
        if (sWorld->getBoolConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING))
            stripLineInvisibleChars(msg);

        if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
        {
            sLog->outError("Player %s (GUID: %u) sent a chatmessage with an invalid link: %s", GetPlayer()->GetName(),
                    GetPlayer()->GetGUIDLow(), msg.c_str());
            if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_KICK))
                KickPlayer();
            return false;
        }
    }

    return true;
}

enum ChatFilterPunishments
{
    CHAT_FILTER_PUNISHMENT_MUTE_10_SEC          = 1,
    CHAT_FILTER_PUNISHMENT_MUTE_30_SEC          = 2,
    CHAT_FILTER_PUNISHMENT_MUTE_1_MIN           = 4,
    CHAT_FILTER_PUNISHMENT_MUTE_2_MIN           = 8,
    CHAT_FILTER_PUNISHMENT_MUTE_5_MIN           = 16,
    CHAT_FILTER_PUNISHMENT_MUTE_10_MIN          = 32,
    CHAT_FILTER_PUNISHMENT_MUTE_20_MIN          = 64,
    CHAT_FILTER_PUNISHMENT_MUTE_30_MIN          = 128,
    CHAT_FILTER_PUNISHMENT_MUTE_1_HOUR          = 256,
    CHAT_FILTER_PUNISHMENT_FREEZE_5_MIN         = 512,
    CHAT_FILTER_PUNISHMENT_FREEZE_10_MIN        = 1024,
    CHAT_FILTER_PUNISHMENT_KICK_PLAYER          = 2048,
    CHAT_FILTER_PUNISHMENT_BAN_PLAYER_1_DAYS    = 4096,
    CHAT_FILTER_PUNISHMENT_BAN_PLAYER_2_DAYS    = 8192,
    CHAT_FILTER_PUNISHMENT_BAN_PLAYER_5_DAYS    = 16384,
    CHAT_FILTER_PUNISHMENT_BAN_PLAYER_7_DAYS    = 32768,
    CHAT_FILTER_PUNISHMENT_STUN_5_MIN           = 65536,
    CHAT_FILTER_PUNISHMENT_STUN_10_MIN          = 131072,

    SPELL_FREEZE                                = 9454,
    SPELL_STUN                                  = 31539,
    SPELL_STUN_SELF_ONE_SEC                     = 65256,
    SPELL_STUN_SELF_VISUAL                      = 18970,

    MAX_ALLOWED_STORED_MESSAGES_IN_CHANNELS     = 10,
};

std::vector<std::pair<uint64 /*guid*/, std::string /*message*/> > messagesInChannel;

void PunishPlayerForBadWord(Player* _sender, uint32 _muteTime = 0, uint16 _banTimeDays = 0, bool _kickPlayer = false)
{
    if (!_sender)
        return;

    if (_muteTime != 0)
    {
        _sender->CastSpell(_sender, SPELL_STUN_SELF_VISUAL, false);
        _sender->CastSpell(_sender, SPELL_STUN_SELF_ONE_SEC, false);
        _sender->GetSession()->m_muteTime = time(NULL) + (_muteTime / 1000);
        _sender->GetSession()->SendNotification("Your chat has been disabled for %u minutes and %u seconds because you've used bad words.", (_muteTime / 60000), ((_muteTime % 60000) / 1000));
        ChatHandler(_sender->GetSession()).PSendSysMessage("Your chat has been disabled for %u minutes and %u seconds because you've used bad words.", (_muteTime / 60000), ((_muteTime % 60000) / 1000));
    }

    if (_kickPlayer)
    {
        _sender->GetSession()->SendNotification("You will be kicked in 3 seconds for using bad words.");
        _sender->m_Events.AddEvent(new kick_player_delay_event(_sender), _sender->m_Events.CalculateTime(3000));
    }

    if (_banTimeDays != 0)
    {
        std::stringstream _duration, _banReason;
        uint64 _banTimeSecs = _banTimeDays * DAY;
        _duration << _banTimeSecs << "s";
        _banReason << "Chat Filter System ban. Duration: " << _banTimeDays << (_banTimeDays == 1 ? " day." : " days.");
        sWorld->BanCharacter(_sender->GetName(), _duration.str(), _banReason.str(), "Chat Filter System");
    }
}

void WorldSession::HandleMessagechatOpcode(WorldPacket & recv_data)
{
    uint32 type;
    uint32 lang;

    recv_data >> type;
    recv_data >> lang;

    if (type >= MAX_CHAT_MSG_TYPE)
    {
        sLog->outError("CHAT: Wrong message type received: %u", type);
        recv_data.rfinish();
        return;
    }

    Player* sender = GetPlayer();

    //sLog->outDebug("CHAT: packet received. type %u, lang %u", type, lang);

    // prevent talking at unknown language (cheating)
    LanguageDesc const* langDesc = GetLanguageDescByID(lang);
    if (!langDesc)
    {
        SendNotification(LANG_UNKNOWN_LANGUAGE);
        recv_data.rfinish();
        return;
    }
    if (langDesc->skill_id != 0 && !sender->HasSkill(langDesc->skill_id))
    {
        // also check SPELL_AURA_COMPREHEND_LANGUAGE (client offers option to speak in that language)
        Unit::AuraEffectList const& langAuras = sender->GetAuraEffectsByType(SPELL_AURA_COMPREHEND_LANGUAGE);
        bool foundAura = false;
        for (Unit::AuraEffectList::const_iterator i = langAuras.begin(); i != langAuras.end(); ++i)
        {
            if ((*i)->GetMiscValue() == int32(lang))
            {
                foundAura = true;
                break;
            }
        }
        if (!foundAura)
        {
            SendNotification(LANG_NOT_LEARNED_LANGUAGE);
            recv_data.rfinish();
            return;
        }
    }

    if (lang == LANG_ADDON)
    {
        if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
        {
            std::string msg = "";
            recv_data >> msg;

            if (msg.empty())
                return;

            sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), lang, msg);
        }

        // Disabled addon channel?
        if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
            return;
    }
    // LANG_ADDON should not be changed nor be affected by flood control
    else
    {
        // send in universal language if player in .gmon mode (ignore spell effects)
        if (sender->isGameMaster())
            lang = LANG_UNIVERSAL;
        else
        {
            // send in universal language in two side iteration allowed mode
            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT))
                lang = LANG_UNIVERSAL;
            else
            {
                switch (type)
                {
                    case CHAT_MSG_PARTY:
                    case CHAT_MSG_PARTY_LEADER:
                    case CHAT_MSG_RAID:
                    case CHAT_MSG_RAID_LEADER:
                    case CHAT_MSG_RAID_WARNING:
                        // allow two side chat at group channel if two side group allowed
                        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP))
                            lang = LANG_UNIVERSAL;
                        break;
                    case CHAT_MSG_GUILD:
                    case CHAT_MSG_OFFICER:
                        // allow two side chat at guild channel if two side guild allowed
                        if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                            lang = LANG_UNIVERSAL;
                        break;
                }
            }

            // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
            Unit::AuraEffectList const& ModLangAuras = sender->GetAuraEffectsByType(SPELL_AURA_MOD_LANGUAGE);
            if (!ModLangAuras.empty())
                lang = ModLangAuras.front()->GetMiscValue();
        }

        if (!sender->CanSpeak())
        {
            std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
            SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
            recv_data.rfinish(); // Prevent warnings
            return;
        }

        if (type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
            sender->UpdateSpeakTime();
    }

    if (sender->HasAura(1852) && type != CHAT_MSG_WHISPER)
    {
        std::string msg="";
        recv_data >> msg;

        SendNotification(GetTrinityString(LANG_GM_SILENCE), sender->GetName());
        return;
    }

    std::string to, channel, msg;
    bool ignoreChecks = false;
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
            recv_data >> msg;
            break;
        case CHAT_MSG_WHISPER:
            recv_data >> to;
            recv_data >> msg;
            break;
        case CHAT_MSG_CHANNEL:
            recv_data >> channel;
            recv_data >> msg;
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            recv_data >> msg;
            ignoreChecks = true;
            break;
    }

    if (!ignoreChecks)
    {
        if (msg.empty())
            return;

        if (ChatHandler(this).ParseCommands(msg.c_str()) > 0)
            return;

        if (!processChatmessageFurtherAfterSecurityChecks(msg, lang))
            return;

        if (msg.empty())
            return;
    }

    bool kickPlayer = false, punishPlayer = false, duplicatedMessage = false;
    uint32 muteTime = 0, banTimeDays = 0, punishment = 0;
    char* message = strdup(msg.c_str());
    char* words = strtok(message, " ,.-()&^%$#@!{}'<>/?|\\=+-_1234567890");
    std::string convertedMsg = msg;
    ObjectMgr::ChatFilterContainer const& censoredWords = sObjectMgr->GetCensoredWords();

    while (words != NULL && !censoredWords.empty())
    {
        for (ObjectMgr::ChatFilterContainer::const_iterator itr = censoredWords.begin(); itr != censoredWords.end(); ++itr)
        {
            if (!stricmp(itr->first.c_str(), words))
            {
                //! Convert everything into lower case
                for (uint16 i = 0; i < convertedMsg.size(); ++i)
                    convertedMsg[i] = tolower(convertedMsg[i]);

                size_t bannedWord = convertedMsg.find(itr->first);

                while (bannedWord != std::string::npos)
                {
                    convertedMsg.replace(bannedWord, itr->first.length(), itr->first.length(), '*');
                    bannedWord = convertedMsg.find(itr->first, bannedWord + 1);
                    punishment = itr->second;
                    punishPlayer = true;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_10_SEC)
                        muteTime += 10000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_30_SEC)
                        muteTime += 30000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_1_MIN)
                       muteTime += 60000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_2_MIN)
                        muteTime += 120000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_5_MIN)
                        muteTime += 300000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_10_MIN)
                        muteTime += 600000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_20_MIN)
                        muteTime += 1200000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_30_MIN)
                        muteTime += 1800000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_MUTE_1_HOUR)
                        muteTime += 3600000;

                    if (punishment & CHAT_FILTER_PUNISHMENT_KICK_PLAYER)
                        kickPlayer = true;

                    if (punishment & CHAT_FILTER_PUNISHMENT_BAN_PLAYER_1_DAYS)
                        banTimeDays += 1;

                    if (punishment & CHAT_FILTER_PUNISHMENT_BAN_PLAYER_2_DAYS)
                        banTimeDays += 2;

                    if (punishment & CHAT_FILTER_PUNISHMENT_BAN_PLAYER_5_DAYS)
                        banTimeDays += 5;

                    if (punishment & CHAT_FILTER_PUNISHMENT_BAN_PLAYER_7_DAYS)
                        banTimeDays += 7;
                }
            }
        }

        words = strtok(NULL, " ,.-()&^%$#@!{}'<>/?|\\=+-_1234567890");
    }

    msg = convertedMsg;

    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        {
            if (type == CHAT_MSG_SAY)
                sender->Say(msg, lang);
            else if (type == CHAT_MSG_EMOTE)
                sender->TextEmote(msg);
            else if (type == CHAT_MSG_YELL)
                sender->Yell(msg, lang);
        } break;
        case CHAT_MSG_WHISPER:
        {
            if (sender->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_WHISPER_REQ), sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ));
                return;
            }

            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }

            Player* receiver = sObjectAccessor->FindPlayerByName(to.c_str());
            bool senderIsPlayer = AccountMgr::IsPlayerAccount(GetSecurity());
            bool receiverIsPlayer = AccountMgr::IsPlayerAccount(receiver ? receiver->GetSession()->GetSecurity() : SEC_PLAYER);
            if (!receiver)
            {
                SendPlayerNotFoundNotice(to);
                return;
            }
	     if (receiver && (senderIsPlayer && !receiverIsPlayer && !receiver->isAcceptWhispers() && !receiver->IsInWhisperWhiteList(sender->GetGUID())))
            {
                receiver->Whisper("SYSTEM: This Game Master does not currently have an open ticket from you and did not receive your whisper. Please submit a new GM Ticket request if you need to speak to a GM. This is an automatic message.", LANG_UNIVERSAL, sender->GetGUID());
                return;
            }

            if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT) && senderIsPlayer && receiverIsPlayer)
                if (GetPlayer()->GetTeam() != receiver->GetTeam())
                {
                    SendWrongFactionNotice();
                    return;
                }

            if (GetPlayer()->HasAura(1852) && !receiver->isGameMaster())
            {
                SendNotification(GetTrinityString(LANG_GM_SILENCE), GetPlayer()->GetName());
                return;
            }

            // If player is a Gamemaster and doesn't accept whisper, we auto-whitelist every player that the Gamemaster is talking to
            if (!senderIsPlayer && !sender->isAcceptWhispers() && !sender->IsInWhisperWhiteList(receiver->GetGUID()))
                sender->AddWhisperWhiteList(receiver->GetGUID());

            GetPlayer()->Whisper(msg, lang, receiver->GetGUID());
        } break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        {
            // if player is in battleground, he cannot say to battleground members by /p
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = _player->GetGroup();
                if (!group || group->isBGGroup())
                    return;
            }

            if (type == CHAT_MSG_PARTY_LEADER && !group->IsLeader(_player->GetGUID()))
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, uint8(type), lang, NULL, 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false, group->GetMemberGroup(GetPlayer()->GetGUID()));
        } break;
        case CHAT_MSG_GUILD:
        {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);

                    guild->BroadcastToGuild(this, false, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        } break;
        case CHAT_MSG_OFFICER:
        {
	 //char mess[1024];

	 switch(GetPlayer()->GetSession()->GetSecurity())
	 {
	 case 0:
	 sWorld->SendWorldText(LANG_PLAYER_CHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 case 1:
	 sWorld->SendWorldText(LANG_VIP_GCHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 case 2:
	 case 3:
	 case 4:
	 case 5:
	 sWorld->SendWorldText(LANG_GM_GCHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 case 6:
	 case 7:
	 case 8:
	 sWorld->SendWorldText(LANG_ADMIN_CHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 case 9:
	 sWorld->SendWorldText(LANG_HADMIN_CHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 case 10:
	 sWorld->SendWorldText(LANG_OWNER_CHAT, GetPlayer()->GetName(), msg.c_str());
	 break;
	 }
        } break;
        case CHAT_MSG_RAID:
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup())
                    return;
            }

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_RAID_LEADER:
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup() || !group->IsLeader(_player->GetGUID()))
                    return;
            }

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID_LEADER, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_RAID_WARNING:
        {
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() || !(group->IsLeader(GetPlayer()->GetGUID()) || group->IsAssistant(GetPlayer()->GetGUID())) || group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID_WARNING, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_BATTLEGROUND:
        {
            //battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_BATTLEGROUND, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_BATTLEGROUND_LEADER:
        {
            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup() || !group->IsLeader(GetPlayer()->GetGUID()))
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_BATTLEGROUND_LEADER, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_CHANNEL:
        {
            if (AccountMgr::IsPlayerAccount(GetSecurity()))
            {
                if (_player->getLevel() < sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ))
                {
                    SendNotification(GetTrinityString(LANG_CHANNEL_REQ), sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ));
                    return;
                }
            }

            if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
            {
                if (Channel* chn = cMgr->GetChannel(channel, _player))
                {
                    for (std::vector<std::pair<uint64, std::string> >::const_iterator itr = messagesInChannel.begin(); itr != messagesInChannel.end(); ++itr)
                    {
                        if (itr->first == sender->GetGUID() && itr->second == msg)
                        {
                            sender->GetSession()->SendNotification("Your message won't be displayed because it's not allowed to flood the channels like that.");
                            duplicatedMessage = true;
                            break; //! Stop looping through elements if we found a 'target' in the vector.
                        }
                    }

                    if (!duplicatedMessage)
                    {
                        sScriptMgr->OnPlayerChat(_player, type, lang, msg, chn);
                        chn->Say(_player->GetGUID(), msg.c_str(), lang);
                        messagesInChannel.push_back(std::make_pair(sender->GetGUID(), msg));

                        //! It's pointless to check for this if the message is never sent to the
                        //! actual channel (so out of the brackets from this if-check), because
                        //! in that case the vector's size wouldn't change anyway.
                        //! Here we nuke out the 'oldest' element from vector messagesInChannel
                        //! if the size of it equals to or is bigger than the max. allowed messages
                        //! to check against.
                        if (messagesInChannel.size() >= MAX_ALLOWED_STORED_MESSAGES_IN_CHANNELS)
                            messagesInChannel.erase(messagesInChannel.begin());
                    }
                }
            }
        } break;
        case CHAT_MSG_AFK:
        {
            if ((msg.empty() || !_player->isAFK()) && !_player->isInCombat())
            {
                if (!_player->isAFK())
                {
                    if (msg.empty())
                        msg  = GetTrinityString(LANG_PLAYER_AFK_DEFAULT);
                    _player->afkMsg = msg;
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);

                _player->ToggleAFK();
                if (_player->isAFK() && _player->isDND())
                    _player->ToggleDND();
            }
        } break;
        case CHAT_MSG_DND:
        {
            if (msg.empty() || !_player->isDND())
            {
                if (!_player->isDND())
                {
                    if (msg.empty())
                        msg = GetTrinityString(LANG_PLAYER_DND_DEFAULT);
                    _player->dndMsg = msg;
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);

                _player->ToggleDND();
                if (_player->isDND() && _player->isAFK())
                    _player->ToggleAFK();
            }
        } break;
        default:
            sLog->outError("CHAT: unknown message type %u, lang: %u", type, lang);
            break;
    }

    //! No need to reset variable punishment because they automatically do that every chatmessage
    if (punishPlayer && !duplicatedMessage)
        PunishPlayerForBadWord(sender, muteTime, banTimeDays, kickPlayer);

    free(message); //! Prevents memoryleaks
}

void WorldSession::HandleEmoteOpcode(WorldPacket & recv_data)
{
    if (!GetPlayer()->isAlive() || GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        return;

    uint32 emote;
    recv_data >> emote;
    sScriptMgr->OnPlayerEmote(GetPlayer(), emote);
    GetPlayer()->HandleEmoteCommand(emote);
}

namespace Trinity
{
    class EmoteChatBuilder
    {
        public:
            EmoteChatBuilder(Player const& player, uint32 text_emote, uint32 emote_num, Unit const* target)
                : i_player(player), i_text_emote(text_emote), i_emote_num(emote_num), i_target(target) {}

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* nam = i_target ? i_target->GetNameForLocaleIdx(loc_idx) : NULL;
                uint32 namlen = (nam ? strlen(nam) : 0) + 1;

                data.Initialize(SMSG_TEXT_EMOTE, (20+namlen));
                data << i_player.GetGUID();
                data << (uint32)i_text_emote;
                data << i_emote_num;
                data << (uint32)namlen;
                if (namlen > 1)
                    data.append(nam, namlen);
                else
                    data << (uint8)0x00;
            }

        private:
            Player const& i_player;
            uint32        i_text_emote;
            uint32        i_emote_num;
            Unit const*   i_target;
    };
}                                                           // namespace Trinity

void WorldSession::HandleTextEmoteOpcode(WorldPacket & recv_data)
{
    if (!GetPlayer()->isAlive())
        return;

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 text_emote, emoteNum;
    uint64 guid;

    recv_data >> text_emote;
    recv_data >> emoteNum;
    recv_data >> guid;

    sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(text_emote);
    if (!em)
        return;

    uint32 emote_anim = em->textid;

    switch (emote_anim)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
            // Only allow text-emotes for "dead" entities (feign death included)
            if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
                break;
             GetPlayer()->HandleEmoteCommand(emote_anim);
             break;
    }

    Unit* unit = ObjectAccessor::GetUnit(*_player, guid);

    CellCoord p = Trinity::ComputeCellCoord(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::EmoteChatBuilder emote_builder(*GetPlayer(), text_emote, emoteNum, unit);
    Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > emote_do(emote_builder);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > > emote_worker(GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), emote_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> >, WorldTypeMapContainer> message(emote_worker);
    cell.Visit(p, message, *GetPlayer()->GetMap(), *GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));

    GetPlayer()->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE, text_emote, 0, unit);

    //Send scripted event call
    if (unit && unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), text_emote);
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recv_data)
{
    uint64 iguid;
    uint8 unk;
    //sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: Received CMSG_CHAT_IGNORED");

    recv_data >> iguid;
    recv_data >> unk;                                       // probably related to spam reporting

    Player* player = ObjectAccessor::FindPlayer(iguid);
    if (!player || !player->GetSession())
        return;

    WorldPacket data;
    ChatHandler::FillMessageData(&data, this, CHAT_MSG_IGNORED, LANG_UNIVERSAL, NULL, GetPlayer()->GetGUID(), GetPlayer()->GetName(), NULL);
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleChannelDeclineInvite(WorldPacket &recvPacket)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Opcode %u", recvPacket.GetOpcode());
}

void WorldSession::SendPlayerNotFoundNotice(std::string name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendPlayerAmbiguousNotice(std::string name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_AMBIGUOUS, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendWrongFactionNotice()
{
    WorldPacket data(SMSG_CHAT_WRONG_FACTION, 0);
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice(ChatRestrictionType restriction)
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 1);
    data << uint8(restriction);
    SendPacket(&data);
}
