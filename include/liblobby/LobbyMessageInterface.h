//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include <s25util/MessageInterface.h>

#include <string>

class LobbyServerList;
class LobbyPlayerList;
class LobbyServerInfo;
class LobbyPlayerInfo;

class LobbyMessageInterface : public MessageInterface
{
public:
    /// Lobby-Login-Nachricht.
    virtual bool OnNMSLobbyLogin(unsigned /*id*/, const unsigned /*revision*/, const std::string& /*user*/,
                                 const std::string& /*pass*/, const std::string& /*version*/)
    {
        return false;
    }

    /// Lobby-Login-Error-Nachricht.
    virtual bool OnNMSLobbyLoginError(unsigned /*id*/, const std::string& /*error*/) { return false; }
    /// Lobby-Logged-In-Nachricht.
    virtual bool OnNMSLobbyLoginDone(unsigned /*id*/, const std::string& /*email*/) { return false; }

    /// Lobby-Player-ID-Nachricht.
    virtual bool OnNMSLobbyID(unsigned /*id*/, unsigned /*playerId*/) { return false; }
    /// Chat-Nachricht.
    virtual bool OnNMSLobbyChat(unsigned /*id*/, const std::string& /*player*/, const std::string& /*text*/)
    {
        return false;
    }

    /// Ping-Nachricht.
    virtual bool OnNMSLobbyPing(unsigned /*id*/) { return false; }
    /// Pong-Nachricht.
    virtual bool OnNMSLobbyPong(unsigned /*id*/) { return false; }

    /// ServerList-Nachricht.
    virtual bool OnNMSLobbyServerList(unsigned /*id*/) { return false; }
    virtual bool OnNMSLobbyServerList(unsigned /*id*/, const LobbyServerList& /*list*/) { return false; }
    /// PlayerList-Nachricht.
    virtual bool OnNMSLobbyPlayerList(unsigned /*id*/) { return false; }
    virtual bool OnNMSLobbyPlayerList(unsigned /*id*/, const LobbyPlayerList& /*onlinePlayers*/,
                                      const LobbyPlayerList& /*ingamePlayers*/)
    {
        return false;
    }

    /// ServerInfo-Nachricht.
    virtual bool OnNMSLobbyServerInfo(unsigned /*id*/, const unsigned& /*server*/) { return false; }
    virtual bool OnNMSLobbyServerInfo(unsigned /*id*/, const LobbyServerInfo& /*info*/) { return false; }

    /// Lobby-Server-Add-Done-Nachricht.
    virtual bool OnNMSLobbyServerAdd(unsigned /*id*/, const LobbyServerInfo& /*info*/) { return false; }
    /// Lobby-Server-Add-Failed-Nachricht.
    virtual bool OnNMSLobbyServerAddFailed(unsigned /*id*/, const std::string& /*error*/) { return false; }
    /// Lobby-Server-Update-Player-Nachricht.
    virtual bool OnNMSLobbyServerUpdatePlayer(unsigned /*id*/, const unsigned /*curplayer*/,
                                              const unsigned /*maxplayer*/)
    {
        return false;
    }
    /// Lobby-Server-Update-Map-Nachricht.
    virtual bool OnNMSLobbyServerUpdateMap(unsigned /*id*/, const std::string& /*map*/) { return false; }
    /// Lobby-Server-Delete-Nachricht.
    virtual bool OnNMSLobbyServerDelete(unsigned /*id*/) { return false; }
    /// Lobby-Server-Join-Nachricht.
    virtual bool OnNMSLobbyServerJoin(unsigned /*id*/) { return false; }
};
