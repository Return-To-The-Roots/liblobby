// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef LOBBYMESSAGEINTERFACE_H_INCLUDED
#define LOBBYMESSAGEINTERFACE_H_INCLUDED

#pragma once

#include "libutil/MessageInterface.h"
#include <string>

class LobbyServerList;
class LobbyPlayerList;
class LobbyServerInfo;
class LobbyPlayerInfo;

class LobbyMessageInterface : public MessageInterface
{
public:
    /// Lobby-Login-Nachricht.
    virtual bool OnNMSLobbyLogin(unsigned /*id*/, const unsigned /*revision*/, const std::string& /*user*/, const std::string& /*pass*/,
                                 const std::string& /*version*/)
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
    virtual bool OnNMSLobbyChat(unsigned /*id*/, const std::string& /*player*/, const std::string& /*text*/) { return false; }

    /// Ping-Nachricht.
    virtual bool OnNMSLobbyPing(unsigned /*id*/) { return false; }
    /// Pong-Nachricht.
    virtual bool OnNMSLobbyPong(unsigned /*id*/) { return false; }

    /// ServerList-Nachricht.
    virtual bool OnNMSLobbyServerList(unsigned /*id*/) { return false; }
    virtual bool OnNMSLobbyServerList(unsigned /*id*/, const LobbyServerList& /*list*/) { return false; }
    /// PlayerList-Nachricht.
    virtual bool OnNMSLobbyPlayerList(unsigned /*id*/) { return false; }
    virtual bool OnNMSLobbyPlayerList(unsigned /*id*/, const LobbyPlayerList& /*onlinePlayers*/, const LobbyPlayerList& /*ingamePlayers*/)
    {
        return false;
    }
    /// RankingList-Nachricht.
    virtual bool OnNMSLobbyRankingList(unsigned /*id*/) { return false; }
    virtual bool OnNMSLobbyRankingList(unsigned /*id*/, const LobbyPlayerList& /*list*/) { return false; }
    /// ServerInfo-Nachricht.
    virtual bool OnNMSLobbyServerInfo(unsigned /*id*/, const unsigned& /*server*/) { return false; }
    virtual bool OnNMSLobbyServerInfo(unsigned /*id*/, const LobbyServerInfo& /*info*/) { return false; }

    /// Lobby-Server-Add-Done-Nachricht.
    virtual bool OnNMSLobbyServerAdd(unsigned /*id*/, const LobbyServerInfo& /*info*/) { return false; }
    /// Lobby-Server-Add-Failed-Nachricht.
    virtual bool OnNMSLobbyServerAddFailed(unsigned /*id*/, const std::string& /*error*/) { return false; }
    /// Lobby-Server-Update-Player-Nachricht.
    virtual bool OnNMSLobbyServerUpdatePlayer(unsigned /*id*/, const unsigned /*curplayer*/, const unsigned /*maxplayer*/) { return false; }
    /// Lobby-Server-Update-Map-Nachricht.
    virtual bool OnNMSLobbyServerUpdateMap(unsigned /*id*/, const std::string& /*map*/) { return false; }
    /// Lobby-Server-Delete-Nachricht.
    virtual bool OnNMSLobbyServerDelete(unsigned /*id*/) { return false; }
    /// Lobby-Server-Join-Nachricht.
    virtual bool OnNMSLobbyServerJoin(unsigned /*id*/) { return false; }
    /// Lobby-Ranking-Info-Nachricht.
    virtual bool OnNMSLobbyRankingInfo(unsigned /*id*/, const LobbyPlayerInfo& /*player*/) { return false; }
};

#endif //! LOBBYMESSAGEINTERFACE_H_INCLUDED
