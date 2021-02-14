//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include "EMailUtils.h"
#include "LobbyMessage.h"
#include "LobbyMessageInterface.h"
#include "LobbyPlayerList.h"
#include "LobbyProtocol.h"
#include "LobbyServerList.h"
#include <s25util/Log.h>

#include <utility>

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
    LobbyMessage_Login() : LobbyMessage(NMS_LOBBY_LOGIN) {} //-V730
    LobbyMessage_Login(const std::string& user, std::string pass, const std::string& version)
        : LobbyMessage(NMS_LOBBY_LOGIN), revision(LOBBYPROTOCOL_VERSION), user(user), pass(std::move(pass)),
          version(version)
    {
        LOG.writeToFile(">>> NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n") % revision % user % "********" % version;
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushUnsignedInt(revision);
        ser.PushLongString(user);
        ser.PushLongString(pass);
        ser.PushLongString(version);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        revision = ser.PopUnsignedInt();

        if(lobbyprotocol::extractVersion(revision) >= 6)
        {
            user = ser.PopLongString();
            pass = ser.PopLongString();
            version = ser.PopLongString();
        }
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_LOGIN(%d, %s, %s, %s)\n") % revision % user % "********" % version;
        return cb->OnNMSLobbyLogin(id, revision, user, pass, version);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Done-Nachricht.
class LobbyMessage_Login_Done : public LobbyMessage
{
private:
    std::string email;

public:
    LobbyMessage_Login_Done() : LobbyMessage(NMS_LOBBY_LOGIN_DONE) {}
    LobbyMessage_Login_Done(std::string email) : LobbyMessage(NMS_LOBBY_LOGIN_DONE), email(std::move(email))
    {
        LOG.writeToFile(">>> NMS_LOBBY_LOGIN_DONE(%s)\n") % s25lobby::getAnonymizedEmail(this->email);
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushLongString(email);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        email = ser.PopLongString();
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_LOGIN_DONE(%s)\n") % s25lobby::getAnonymizedEmail(email);
        return cb->OnNMSLobbyLoginDone(id, email);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Login-Error-Nachricht.
class LobbyMessage_Login_Error : public LobbyMessage
{
private:
    std::string error;

public:
    LobbyMessage_Login_Error() : LobbyMessage(NMS_LOBBY_LOGIN_ERROR) {}
    LobbyMessage_Login_Error(const std::string& error) : LobbyMessage(NMS_LOBBY_LOGIN_ERROR), error(error)
    {
        LOG.writeToFile(">>> NMS_LOBBY_LOGIN_ERROR(%s)\n") % error;
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushLongString(error);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        error = ser.PopLongString();
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_LOGIN_ERROR(%s)\n") % error;
        return cb->OnNMSLobbyLoginError(id, error);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende ServerList-Nachricht.
class LobbyMessage_ServerList : public LobbyMessage
{
private:
    bool isRequest;
    LobbyServerList list;

public:
    LobbyMessage_ServerList() : LobbyMessage(NMS_LOBBY_SERVERLIST) {} //-V730
    LobbyMessage_ServerList(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_SERVERLIST), isRequest(true)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVERLIST\n");
    }
    LobbyMessage_ServerList(const LobbyServerList& list)
        : LobbyMessage(NMS_LOBBY_SERVERLIST), isRequest(false), list(list)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVERLIST(%d)\n") % list.size();
        for(unsigned i = 0; i < list.size(); ++i)
        {
            const LobbyServerInfo* server = list.getElement(i);
            LOG.writeToFile("    %d: %d %s %s:%d %s %d %s %d %d\n") % i % server->getId() % server->getName()
              % server->getHost() % server->getPort() % server->getVersion() % server->getPing() % server->getMap()
              % server->getCurPlayers() % server->getMaxPlayers();
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
        } else
            isRequest = true;
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        if(isRequest)
        {
            LOG.writeToFile("<<< NMS_LOBBY_SERVERLIST\n");
            return cb->OnNMSLobbyServerList(id);
        } else
        {
            LOG.writeToFile("<<< NMS_LOBBY_SERVERLIST(%d)\n") % list.size();
            for(unsigned i = 0; i < list.size(); ++i)
            {
                const LobbyServerInfo* server = list.getElement(i);
                LOG.writeToFile("    %d: %d %s %s:%d %s %d %s %d %d\n") % i % server->getId() % server->getName()
                  % server->getHost() % server->getPort() % server->getVersion() % server->getPing() % server->getMap()
                  % server->getCurPlayers() % server->getMaxPlayers();
            }
            return cb->OnNMSLobbyServerList(id, list);
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
    LobbyMessage_ServerInfo() : LobbyMessage(NMS_LOBBY_SERVERINFO) {} //-V730
    LobbyMessage_ServerInfo(const unsigned server) : LobbyMessage(NMS_LOBBY_SERVERINFO), isRequest(true), server(server)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVERINFO(%d)\n") % server;
    }
    LobbyMessage_ServerInfo(const LobbyServerInfo& info)
        : LobbyMessage(NMS_LOBBY_SERVERINFO), isRequest(false), info(info)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVERINFO\n");
        LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost()
          % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers()
          % info.getMaxPlayers();
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

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        if(isRequest) // Anfrage
        {
            LOG.writeToFile("<<< NMS_LOBBY_SERVERINFO(%d)\n") % server;
            return cb->OnNMSLobbyServerInfo(id, server);
        } else // Antwort
        {
            LOG.writeToFile("<<< NMS_LOBBY_SERVERINFO\n");
            LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost()
              % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers()
              % info.getMaxPlayers();
            return cb->OnNMSLobbyServerInfo(id, info);
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
    LobbyMessage_PlayerList() : LobbyMessage(NMS_LOBBY_PLAYERLIST) {} //-V730
    LobbyMessage_PlayerList(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_PLAYERLIST), isRequest(true)
    {
        LOG.writeToFile(">>> NMS_LOBBY_PLAYERLIST\n");
    }
    LobbyMessage_PlayerList(const LobbyPlayerList& list)
        : LobbyMessage(NMS_LOBBY_PLAYERLIST), isRequest(false), list(list)
    {
        LOG.writeToFile(">>> NMS_LOBBY_PLAYERLIST(%d)\n") % list.size();

        for(unsigned i = 0; i < list.size(); ++i)
        {
            const LobbyPlayerInfo* player = list.getElement(i);
            LOG.writeToFile("    %d: %d %s %s\n") % i % player->getId() % player->getName() % player->getVersion();
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
                for(auto& ingamePlayer : ingamePlayers)
                    ingamePlayer.isIngame = true;
            } else
                ingamePlayers.clear();
        }
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        if(isRequest) // Anfrage
        {
            LOG.writeToFile("<<< NMS_LOBBY_PLAYERLIST\n");
            return cb->OnNMSLobbyPlayerList(id);
        } else // Antwort
        {
            LOG.writeToFile("<<< NMS_LOBBY_PLAYERLIST(%d)\n") % list.size();
            for(unsigned i = 0; i < list.size(); ++i)
            {
                const LobbyPlayerInfo* player = list.getElement(i);
                LOG.writeToFile("    %d: %d %s %s\n") % i % player->getId() % player->getName() % player->getVersion();
            }

            return cb->OnNMSLobbyPlayerList(id, list, ingamePlayers);
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
    LobbyMessage_Id() : LobbyMessage(NMS_LOBBY_ID) {} //-V730
    LobbyMessage_Id(const unsigned playerId) : LobbyMessage(NMS_LOBBY_ID), playerId(playerId)
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

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_ID(%d)\n") % playerId;

        return cb->OnNMSLobbyID(id, playerId);
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
    LobbyMessage_Chat() : LobbyMessage(NMS_LOBBY_CHAT) {}
    LobbyMessage_Chat(const std::string& text) : LobbyMessage(NMS_LOBBY_CHAT), text(text)
    {
        LOG.writeToFile(">>> NMS_LOBBY_CHAT(%s)\n") % text;
    }
    LobbyMessage_Chat(const std::string& player, const std::string& text)
        : LobbyMessage(NMS_LOBBY_CHAT), player(player), text(text)
    {
        LOG.writeToFile(">>> NMS_LOBBY_CHAT(%s, %s)\n") % player % text;
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushLongString(player);
        ser.PushLongString(text);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        player = ser.PopLongString();
        text = ser.PopLongString();
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_CHAT(%s, %s)\n") % player % text;
        return cb->OnNMSLobbyChat(id, player, text);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Ping-Nachricht.
class LobbyMessage_Ping : public LobbyMessage
{
public:
    LobbyMessage_Ping() : LobbyMessage(NMS_LOBBY_PING) {}
    LobbyMessage_Ping(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_PING)
    {
        // LOG.writeToFile(">>> NMS_LOBBY_PING\n");
    }
    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        // LOG.writeToFile("<<< NMS_LOBBY_PING\n");
        return cb->OnNMSLobbyPing(id);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Pong-Nachricht.
class LobbyMessage_Pong : public LobbyMessage
{
public:
    LobbyMessage_Pong() : LobbyMessage(NMS_LOBBY_PONG) {}
    LobbyMessage_Pong(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_PONG)
    {
        // LOG.writeToFile(">>> NMS_LOBBY_PONG\n");
    }
    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        // LOG.writeToFile("<<< NMS_LOBBY_PONG\n");
        return cb->OnNMSLobbyPong(id);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ein/ausgehende Server-Add-Nachricht.
class LobbyMessage_Server_Add : public LobbyMessage
{
private:
    LobbyServerInfo info;

public:
    LobbyMessage_Server_Add() : LobbyMessage(NMS_LOBBY_SERVER_ADD) {}
    LobbyMessage_Server_Add(const LobbyServerInfo& info) : LobbyMessage(NMS_LOBBY_SERVER_ADD), info(info)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVER_ADD\n");
        LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost()
          % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers()
          % info.getMaxPlayers();
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

    bool run(MessageInterface* callback, unsigned id) override
    {
        LOG.writeToFile("<<< NMS_LOBBY_SERVER_ADD\n");
        LOG.writeToFile("    %d %s %s:%d %s %d %s %d %d\n") % info.getId() % info.getName() % info.getHost()
          % info.getPort() % info.getVersion() % info.getPing() % info.getMap() % info.getCurPlayers()
          % info.getMaxPlayers();

        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        return cb->OnNMSLobbyServerAdd(id, info);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// eingehende Server-Add-Failed.
class LobbyMessage_Server_Add_Failed : public LobbyMessage
{
private:
    std::string error;

public:
    LobbyMessage_Server_Add_Failed() : LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED) {}
    LobbyMessage_Server_Add_Failed(const std::string& error) : LobbyMessage(NMS_LOBBY_SERVER_ADD_FAILED), error(error)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVER_ADD_FAILED(%s)\n") % error;
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushLongString(error);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        error = ser.PopLongString();
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        LOG.writeToFile("<<< NMS_LOBBY_SERVER_ADD_FAILED(%s)\n") % error;
        return cb->OnNMSLobbyServerAddFailed(id, error);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Delete-Nachricht.
class LobbyMessage_Server_Delete : public LobbyMessage
{
public:
    LobbyMessage_Server_Delete() : LobbyMessage(NMS_LOBBY_SERVER_DELETE) {}
    LobbyMessage_Server_Delete(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_SERVER_DELETE)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVER_DELETE\n");
    }
    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        LOG.writeToFile("<<< NMS_LOBBY_SERVER_DELETE\n");
        return cb->OnNMSLobbyServerDelete(id);
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
    LobbyMessage_Server_Update_Player() : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER) {} //-V730
    LobbyMessage_Server_Update_Player(const unsigned curplayer, const unsigned maxplayer)
        : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_PLAYER), curplayer(curplayer), maxplayer(maxplayer)
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

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);
        LOG.writeToFile("<<< NMS_LOBBY_SERVER_UPDATE_PLAYER(%d,%d)\n") % curplayer % maxplayer;
        return cb->OnNMSLobbyServerUpdatePlayer(id, curplayer, maxplayer);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Update-Map-Nachricht.
class LobbyMessage_Server_Update_Map : public LobbyMessage
{
private:
    std::string map;

public:
    LobbyMessage_Server_Update_Map() : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP) {}
    LobbyMessage_Server_Update_Map(const std::string& map) : LobbyMessage(NMS_LOBBY_SERVER_UPDATE_MAP), map(map)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n") % map;
    }

    void Serialize(Serializer& ser) const override
    {
        LobbyMessage::Serialize(ser);
        ser.PushLongString(map);
    }

    void Deserialize(Serializer& ser) override
    {
        LobbyMessage::Deserialize(ser);
        map = ser.PopLongString();
    }

    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        LOG.writeToFile("<<< NMS_LOBBY_SERVER_UPDATE_MAP(%s)\n") % map;
        return cb->OnNMSLobbyServerUpdateMap(id, map);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// ausgehende Server-Join-Nachricht.
class LobbyMessage_Server_Join : public LobbyMessage
{
public:
    LobbyMessage_Server_Join() : LobbyMessage(NMS_LOBBY_SERVER_JOIN) {}
    LobbyMessage_Server_Join(bool /*reserved*/) : LobbyMessage(NMS_LOBBY_SERVER_JOIN)
    {
        LOG.writeToFile(">>> NMS_LOBBY_SERVER_JOIN\n");
    }
    bool run(MessageInterface* callback, unsigned id) override
    {
        auto* cb = static_cast<LobbyMessageInterface*>(callback);

        LOG.writeToFile("<<< NMS_LOBBY_SERVER_JOIN\n");
        return cb->OnNMSLobbyServerJoin(id);
    }
};
