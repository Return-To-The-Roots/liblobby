// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef LOBBYMESSAGES_H_INCLUDED
#define LOBBYMESSAGES_H_INCLUDED

#pragma once

#include "LobbyMessage.h"
#include "LobbyMessageInterface.h"
#include "LobbyServerList.h"
#include "LobbyPlayerList.h"
#include "LobbyProtocol.h"
#include "libutil/Log.h"

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Login-Nachricht
class LobbyMessage_Login : public LobbyMessage
{
    private:
        unsigned revision;
        std::string user;
        std::string pass;
        std::string version;

    public:
        LobbyMessage_Login(): LobbyMessage(NMS_LOBBY_LOGIN) { } //-V730
        LobbyMessage_Login(const std::string& user, const std::string& pass, const std::string& version):
            LobbyMessage(NMS_LOBBY_LOGIN), user(user), pass(pass), version(version)
        {
            LOG.writeToFile(">>> NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n") % LOBBYPROTOCOL_VERSION % user % "********" % version;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushUnsignedInt(LOBBYPROTOCOL_VERSION);
            ser.PushString(user);
            ser.PushString(pass);
            ser.PushString(version);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            unsigned char rev[4];
            ser.PopRawData(rev, 4);

            // haben wir eine gültige Revision erhalten?
            if(rev[0] != 0xFF || rev[3] != 0xFF)
                revision = 0;
            else
                revision = htonl(*((unsigned*)rev));

            if(revision == LOBBYPROTOCOL_VERSION)
            {
                user = ser.PopString();
                pass = ser.PopString();
                version = ser.PopString();
            }
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n") % revision % user % "********" % version;
            cb->OnNMSLobbyLogin(id, revision, user, pass, version);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Done-Nachricht.
class LobbyMessage_Login_Done : public LobbyMessage
{
    private:
        std::string email;

    public:
        LobbyMessage_Login_Done(): LobbyMessage(NMS_LOBBY_LOGIN_DONE) { }
        LobbyMessage_Login_Done(const std::string& email): LobbyMessage(NMS_LOBBY_LOGIN_DONE), email(email)
        {
            LOG.writeToFile(">>> NMS_LOBBY_LOGIN_DONE(%s)\n") % email;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(email);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            email = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_LOGIN_DONE(%s)\n") % email;
            cb->OnNMSLobbyLoginDone(id, email);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Error-Nachricht.
class LobbyMessage_Login_Error : public LobbyMessage
{
    private:
        std::string error;

    public:
        LobbyMessage_Login_Error(): LobbyMessage(NMS_LOBBY_LOGIN_ERROR) { }
        LobbyMessage_Login_Error(const std::string& error): LobbyMessage(NMS_LOBBY_LOGIN_ERROR), error(error)
        {
            LOG.writeToFile(">>> NMS_LOBBY_LOGIN_ERROR(%s)\n") % error;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(error);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            error = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_LOGIN_ERROR(%s)\n") % error;
            cb->OnNMSLobbyLoginError(id, error);
        }
};

/** eingehende Login-Error-Nachricht. (0.6 compat mode)
 *  sollte niemals empfangen werden (außer es ist ein pre 0.6 client!)
 */
class LobbyMessage_Login_Error06 : public LobbyMessage
{
    std::string error;
    public:
        LobbyMessage_Login_Error06(const std::string& error): LobbyMessage(NMS_LOBBY_LOGIN_ERROR), error(error)
        {
            LOG.writeToFile(">>> NMS_LOBBY_LOGIN_ERROR(compat: %s)\n") % error;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushRawData(error.c_str(), static_cast<unsigned>(error.size()));
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
        }

        void run(MessageInterface*  /*callback*/, unsigned  /*id*/) override {}
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Register-Nachricht.
class LobbyMessage_Register : public LobbyMessage
{
    private:
        unsigned revision;
        std::string user;
        std::string pass;
        std::string email;

    public:
        LobbyMessage_Register(): LobbyMessage(NMS_LOBBY_REGISTER) {} //-V730
        LobbyMessage_Register(const std::string& user, const std::string& pass, const std::string& email):
            LobbyMessage(NMS_LOBBY_REGISTER), user(user), pass(pass), email(email)
        {
            LOG.writeToFile(">>> NMS_LOBBY_REGISTER(%d, %s, %s, %s)\n") % revision % user % "********" % email;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushUnsignedInt(LOBBYPROTOCOL_VERSION);
            ser.PushString(user);
            ser.PushString(pass);
            ser.PushString(email);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            unsigned char rev[4];
            ser.PopRawData(rev, 4);

            // haben wir eine gültige Revision erhalten?
            if(rev[0] != 0xFF || rev[3] != 0xFF)
                revision = 0;
            else
                revision = htonl(*((unsigned*)rev));

            if(revision == LOBBYPROTOCOL_VERSION)
            {
                user = ser.PopString();
                pass = ser.PopString();
                email = ser.PopString();
            }
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_REGISTER(%d, %s, %s, %s)\n") % revision % user % "********" % email;
            cb->OnNMSLobbyRegister(id, revision, user, pass, email);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Register-Done-Nachricht.
class LobbyMessage_Register_Done : public LobbyMessage
{
    public:
        LobbyMessage_Register_Done(): LobbyMessage(NMS_LOBBY_REGISTER_DONE) {}
        LobbyMessage_Register_Done(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_REGISTER_DONE)
        {
            LOG.writeToFile(">>> NMS_LOBBY_REGISTER_DONE\n");
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            LOG.writeToFile("<<< NMS_LOBBY_REGISTER_DONE\n");
            cb->OnNMSLobbyRegisterDone(id);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Register-Error-Nachricht
class LobbyMessage_Register_Error : public LobbyMessage
{
    private:
        std::string error;

    public:
        LobbyMessage_Register_Error(): LobbyMessage(NMS_LOBBY_REGISTER_ERROR) {}
        LobbyMessage_Register_Error(const std::string& error): LobbyMessage(NMS_LOBBY_LOGIN_ERROR), error(error)
        {
            LOG.writeToFile(">>> NMS_LOBBY_REGISTER_ERROR(%s)\n") % error;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(error);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            error = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_REGISTER_ERROR(%s)\n") % error;
            cb->OnNMSLobbyRegisterError(id, error);
        }
};

/** eingehende Register-Error-Nachricht. (0.6 compat mode)
 *  sollte niemals empfangen werden (außer es ist ein pre 0.6 client!)
 */
class LobbyMessage_Register_Error06 : public LobbyMessage
{
    std::string error;
    public:
        LobbyMessage_Register_Error06(const std::string& error): LobbyMessage(NMS_LOBBY_LOGIN_ERROR), error(error)
        {
            LOG.writeToFile(">>> NMS_LOBBY_REGISTER_ERROR(compat: %s)\n") % error;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushRawData(error.c_str(), static_cast<unsigned>(error.size()));
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
        }

        void run(MessageInterface*  /*callback*/, unsigned  /*id*/) override {}
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende ServerList-Nachricht.
class LobbyMessage_ServerList : public LobbyMessage
{
    private:
        bool isRequest;
        LobbyServerList list;

    public:
        LobbyMessage_ServerList(): LobbyMessage(NMS_LOBBY_SERVERLIST) {} //-V730
        LobbyMessage_ServerList(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_SERVERLIST), isRequest(true)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVERLIST\n");
        }
        LobbyMessage_ServerList(const LobbyServerList& list): LobbyMessage(NMS_LOBBY_SERVERLIST), isRequest(false), list(list)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVERLIST(%d)\n") % list.getCount();
            for(unsigned i = 0; i < list.getCount(); ++i)
            {
                const LobbyServerInfo* server = list.getElement(i);
                LOG.writeToFile("    %d: %d %s %s:%d %s %d %s %d %d\n") % i % server->getId() % server->getName() % server->getHost() % server->getPort() % server->getVersion() % server->getPing() % server->getMap() % server->getCurPlayers() % server->getMaxPlayers();
            }
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            if(!isRequest)
                list.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            if(ser.GetBytesLeft() > 0)
            {
                isRequest = false;
                list.deserialize(ser);
            }else
                isRequest = true;
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            if(isRequest)
            {
                LOG.writeToFile("<<< NMS_LOBBY_SERVERLIST\n");
                cb->OnNMSLobbyServerList(id);
            }else
            {
                LOG.writeToFile("<<< NMS_LOBBY_SERVERLIST(%d)\n") % list.getCount();
                for(unsigned i = 0; i < list.getCount(); ++i)
                {
                    const LobbyServerInfo* server = list.getElement(i);
                    LOG.writeToFile("    %d: %d %s %s:%d %s %d %s %d %d\n") % i % server->getId() % server->getName() % server->getHost() % server->getPort() % server->getVersion() % server->getPing() % server->getMap() % server->getCurPlayers() % server->getMaxPlayers();
                }
                cb->OnNMSLobbyServerList(id, list);
            }
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende ServerInfo-Nachricht.
class LobbyMessage_ServerInfo : public LobbyMessage
{
    private:
        bool isRequest;
        LobbyServerInfo info;
        unsigned server;

    public:
        LobbyMessage_ServerInfo(): LobbyMessage(NMS_LOBBY_SERVERINFO) {} //-V730
        LobbyMessage_ServerInfo(const unsigned server): LobbyMessage(NMS_LOBBY_SERVERINFO), isRequest(true), server(server)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVERINFO(%d)\n") % server;
        }
        LobbyMessage_ServerInfo(const LobbyServerInfo& info): LobbyMessage(NMS_LOBBY_SERVERINFO), isRequest(false), info(info)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVERINFO\n");
            LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost() % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers() % info.getMaxPlayers();
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            if(isRequest)
                ser.PushUnsignedInt(server);
            else
                info.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            isRequest = ser.GetBytesLeft() == sizeof(unsigned);
            if(isRequest)
                server = ser.PopUnsignedInt();
            else
                info = LobbyServerInfo(0, ser);
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            if(isRequest) // Anfrage
            {
                LOG.writeToFile("<<< NMS_LOBBY_SERVERINFO(%d)\n") % server;
                cb->OnNMSLobbyServerInfo(id, server);
            }
            else // Antwort
            {
                LOG.writeToFile("<<< NMS_LOBBY_SERVERINFO\n");
                LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost() % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers() % info.getMaxPlayers();
                cb->OnNMSLobbyServerInfo(id, info);
            }
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Rankinglist-Nachricht.
class LobbyMessage_RankingList : public LobbyMessage
{
    private:
        bool isRequest;
        LobbyPlayerList list;

    public:
        LobbyMessage_RankingList(): LobbyMessage(NMS_LOBBY_RANKINGLIST) {} //-V730
        LobbyMessage_RankingList(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_RANKINGLIST), isRequest(true)
        {
            LOG.writeToFile(">>> NMS_LOBBY_RANKINGLIST\n");
        }
        LobbyMessage_RankingList(const LobbyPlayerList& list): LobbyMessage(NMS_LOBBY_RANKINGLIST), isRequest(false), list(list)
        {
            LOG.writeToFile(">>> NMS_LOBBY_RANKINGLIST(%d)\n") % list.getCount();

            for(unsigned i = 0; i < list.getCount(); ++i)
            {
                const LobbyPlayerInfo* player = list.getElement(i);
                LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % i % player->getId() % player->getName() % player->getVersion() % player->getPunkte() % player->getGewonnen() % player->getVerloren();
            }
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            if(!isRequest)
                list.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            isRequest = ser.GetBytesLeft() == 0;
            if(!isRequest)
                list.deserialize(ser);
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            if(isRequest) // Anfrage
            {
                LOG.writeToFile("<<< NMS_LOBBY_RANKINGLIST\n");
                cb->OnNMSLobbyRankingList(id);
            }
            else // Antwort
            {
                LOG.writeToFile("<<< NMS_LOBBY_RANKINGLIST(%d)\n") % list.getCount();
                for(unsigned i = 0; i < list.getCount(); ++i)
                {
                    const LobbyPlayerInfo* player = list.getElement(i);
                    LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % i % player->getId() % player->getName() % player->getVersion() % player->getPunkte() % player->getGewonnen() % player->getVerloren();
                }

                cb->OnNMSLobbyRankingList(id, list);
            }
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende PlayerList-Nachricht.
class LobbyMessage_PlayerList : public LobbyMessage
{
    private:
        bool isRequest;
        LobbyPlayerList list;
        LobbyPlayerList ingamePlayers;

    public:
        LobbyMessage_PlayerList(): LobbyMessage(NMS_LOBBY_PLAYERLIST) { } //-V730
        LobbyMessage_PlayerList(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_PLAYERLIST), isRequest(true)
        {
            LOG.writeToFile(">>> NMS_LOBBY_PLAYERLIST\n");
        }
        LobbyMessage_PlayerList(const LobbyPlayerList& list): LobbyMessage(NMS_LOBBY_PLAYERLIST), isRequest(false), list(list)
        {
            LOG.writeToFile(">>> NMS_LOBBY_PLAYERLIST(%d)\n") % list.getCount();

            for(unsigned i = 0; i < list.getCount(); ++i)
            {
                const LobbyPlayerInfo* player = list.getElement(i);
                LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % i % player->getId() % player->getName() % player->getVersion() % player->getPunkte() % player->getGewonnen() % player->getVerloren();
            }

        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            if(!isRequest)
                list.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            isRequest = ser.GetBytesLeft() == 0;
            if(!isRequest)
            {
                list.deserialize(ser);
                if(ser.GetBytesLeft() > 0)
                {
                    ingamePlayers.deserialize(ser);
                    for(LobbyPlayerList::iterator it = ingamePlayers.begin(); it != ingamePlayers.end(); ++it)
                        it->isIngame = true;
                } else
                    ingamePlayers.clear();
            }
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            if(isRequest) // Anfrage
            {
                LOG.writeToFile("<<< NMS_LOBBY_PLAYERLIST\n");
                cb->OnNMSLobbyPlayerList(id);
            }
            else // Antwort
            {
                LOG.writeToFile("<<< NMS_LOBBY_PLAYERLIST(%d)\n") % list.getCount();
                for(unsigned i = 0; i < list.getCount(); ++i)
                {
                    const LobbyPlayerInfo* player = list.getElement(i);
                    LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % i % player->getId() % player->getName() % player->getVersion() % player->getPunkte() % player->getGewonnen() % player->getVerloren();
                }

                cb->OnNMSLobbyPlayerList(id, list, ingamePlayers);
            }
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Id-Nachricht.
class LobbyMessage_Id : public LobbyMessage
{
    private:
        unsigned playerId;

    public:
        LobbyMessage_Id(): LobbyMessage(NMS_LOBBY_ID) {} //-V730
        LobbyMessage_Id(const unsigned playerId): LobbyMessage(NMS_LOBBY_ID), playerId(playerId)
        {
            LOG.writeToFile(">>> NMS_LOBBY_ID(%d)\n") % playerId;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushUnsignedInt(playerId);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            playerId = ser.PopUnsignedInt();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_ID(%d)\n") % playerId;

            cb->OnNMSLobbyID(id, playerId);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Chat-Nachricht.
class LobbyMessage_Chat : public LobbyMessage
{
    private:
        std::string player;
        std::string text;

    public:
        LobbyMessage_Chat(): LobbyMessage(NMS_LOBBY_CHAT) { }
        LobbyMessage_Chat(const std::string& text): LobbyMessage(NMS_LOBBY_CHAT), player(""), text(text)
        {
            LOG.writeToFile(">>> NMS_LOBBY_CHAT(%s)\n") % text;
        }
        LobbyMessage_Chat(const std::string& player, const std::string& text): LobbyMessage(NMS_LOBBY_CHAT), player(player), text(text)
        {
            LOG.writeToFile(">>> NMS_LOBBY_CHAT(%s, %s)\n") % player % text;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(player);
            ser.PushString(text);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            player = ser.PopString();
            text = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_CHAT(%s, %s)\n") % player % text;
            cb->OnNMSLobbyChat(id, player, text);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Ping-Nachricht.
class LobbyMessage_Ping : public LobbyMessage
{
    public:
        LobbyMessage_Ping(): LobbyMessage(NMS_LOBBY_PING) { }
        LobbyMessage_Ping(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_PING)
        {
            //LOG.writeToFile(">>> NMS_LOBBY_PING\n");
        }
        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            //LOG.writeToFile("<<< NMS_LOBBY_PING\n");
            cb->OnNMSLobbyPing(id);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Pong-Nachricht.
class LobbyMessage_Pong : public LobbyMessage
{
    public:
        LobbyMessage_Pong(): LobbyMessage(NMS_LOBBY_PONG) { }
        LobbyMessage_Pong(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_PONG)
        {
            //LOG.writeToFile(">>> NMS_LOBBY_PONG\n");
        }
        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            //LOG.writeToFile("<<< NMS_LOBBY_PONG\n");
            cb->OnNMSLobbyPong(id);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Server-Add-Nachricht.
class LobbyMessage_Server_Add : public LobbyMessage
{
    private:
        LobbyServerInfo info;

    public:
        LobbyMessage_Server_Add(): LobbyMessage(NMS_LOBBY_SERVER_ADD) { }
        LobbyMessage_Server_Add(const LobbyServerInfo& info): LobbyMessage(NMS_LOBBY_SERVER_ADD), info(info)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_ADD\n");
            LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost() % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers() % info.getMaxPlayers();
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            info.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            info = LobbyServerInfo(0, ser);
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LOG.writeToFile("<<< NMS_LOBBY_SERVER_ADD\n");
            LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost() % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers() % info.getMaxPlayers();

            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            cb->OnNMSLobbyServerAdd(id, info);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Server-Add-Failed.
class LobbyMessage_Server_Add_Failed : public LobbyMessage
{
    private:
        std::string error;

    public:
        LobbyMessage_Server_Add_Failed(): LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED) { }
        LobbyMessage_Server_Add_Failed(const std::string& error): LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED), error(error)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_ADD_FAILED(%s)\n") % error;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(error);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            error = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            LOG.writeToFile("<<< NMS_LOBBY_SERVER_ADD_FAILED(%s)\n") % error;
            cb->OnNMSLobbyServerAddFailed(id, error);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Delete-Nachricht.
class LobbyMessage_Server_Delete : public LobbyMessage
{
    public:
        LobbyMessage_Server_Delete(): LobbyMessage(NMS_LOBBY_SERVER_DELETE) { }
        LobbyMessage_Server_Delete(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_SERVER_DELETE)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_DELETE\n");
        }
        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            LOG.writeToFile("<<< NMS_LOBBY_SERVER_DELETE\n");
            cb->OnNMSLobbyServerDelete(id);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Update-Player-Nachricht.
class LobbyMessage_Server_Update_Player : public LobbyMessage
{
    private:
        unsigned curplayer;
        unsigned maxplayer;

    public:
        LobbyMessage_Server_Update_Player(): LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER) { } //-V730
        LobbyMessage_Server_Update_Player(const unsigned curplayer, const unsigned maxplayer):
            LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER), curplayer(curplayer), maxplayer(maxplayer)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_UPDATE_PLAYER(%d,%d)\n") % curplayer % maxplayer;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushUnsignedInt(curplayer);
            ser.PushUnsignedInt(maxplayer);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            curplayer = ser.PopUnsignedInt();
            maxplayer = ser.PopUnsignedInt();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_SERVER_UPDATE_PLAYER(%d,%d)\n") % curplayer % maxplayer;
            cb->OnNMSLobbyServerUpdatePlayer(id, curplayer, maxplayer);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Update-Map-Nachricht.
class LobbyMessage_Server_Update_Map : public LobbyMessage
{
    private:
        std::string map;

    public:
        LobbyMessage_Server_Update_Map(): LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP) { }
        LobbyMessage_Server_Update_Map(const std::string& map): LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP), map(map)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n") % map;
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushString(map);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            map = ser.PopString();
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            LOG.writeToFile("<<< NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n") % map;
            cb->OnNMSLobbyServerUpdateMap(id, map);
        }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Join-Nachricht.
class LobbyMessage_Server_Join : public LobbyMessage
{
    public:
        LobbyMessage_Server_Join(): LobbyMessage(NMS_LOBBY_SERVER_JOIN) { }
        LobbyMessage_Server_Join(bool  /*reserved*/): LobbyMessage(NMS_LOBBY_SERVER_JOIN)
        {
            LOG.writeToFile(">>> NMS_LOBBY_SERVER_JOIN\n");
        }
        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);

            LOG.writeToFile("<<< NMS_LOBBY_SERVER_JOIN\n");
            cb->OnNMSLobbyServerJoin(id);
        }
};


///////////////////////////////////////////////////////////////////////////////
/// aus/eingehende Ranking-Info-Nachricht.
class LobbyMessage_Lobby_Ranking_Info : public LobbyMessage
{
    public:
        bool isRequest;
        std::string name;
        LobbyPlayerInfo player;

    public:
        LobbyMessage_Lobby_Ranking_Info(): LobbyMessage(NMS_LOBBY_RANKING_INFO) { } //-V730
        LobbyMessage_Lobby_Ranking_Info(const std::string& name): LobbyMessage(NMS_LOBBY_RANKING_INFO), isRequest(true), name(name)
        {
            LOG.writeToFile(">>> NMS_LOBBY_RANKING_INFO(%s)\n") % name;
        }
        LobbyMessage_Lobby_Ranking_Info(const LobbyPlayerInfo& player): LobbyMessage(NMS_LOBBY_RANKING_INFO), isRequest(false), player(player)
        {
            LOG.writeToFile(">>> NMS_LOBBY_RANKING_INFO(%s)\n") % player.getName();
            LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % 0 % player.getId() % player.getName() % player.getVersion() % player.getPunkte() % player.getGewonnen() % player.getVerloren();
        }

        void Serialize(Serializer& ser) const override
        {
            LobbyMessage::Serialize(ser);
            ser.PushBool(isRequest);
            if(isRequest)
                ser.PushString(name);
            else
                player.serialize(ser);
        }

        void Deserialize(Serializer& ser) override
        {
            LobbyMessage::Deserialize(ser);
            isRequest = ser.PopBool();
            if(isRequest) // Anfrage
                player.setName(ser.PopString());
            else
                player.deserialize(ser);
        }

        void run(MessageInterface* callback, unsigned id) override
        {
            LobbyMessageInterface* cb = dynamic_cast<LobbyMessageInterface*>(callback);
            LOG.writeToFile("<<< NMS_LOBBY_RANKING_INFO(%s)\n") % player.getName();
            LOG.writeToFile("    %d: %d %s %s %d %d %d\n") % 0 % player.getId() % player.getName() % player.getVersion() % player.getPunkte() % player.getGewonnen() % player.getVerloren();

            cb->OnNMSLobbyRankingInfo(id, player);
        }
};

#endif //!LOBBYMESSAGES_H_INCLUDED
