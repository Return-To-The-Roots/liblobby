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
        virtual void OnNMSLobbyLogin(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& version){}
        /// Lobby-Login-Error-Nachricht.
        virtual void OnNMSLobbyLoginError(unsigned id, const std::string& error){}
        /// Lobby-Logged-In-Nachricht.
        virtual void OnNMSLobbyLoginDone(unsigned id, const std::string& email){}

        /// Lobby-Register-Nachricht.
        virtual void OnNMSLobbyRegister(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& email){}
        /// Lobby-Register-Error-Nachricht.
        virtual void OnNMSLobbyRegisterError(unsigned id, const std::string& error){}
        /// Lobby-Register-Done-Nachricht.
        virtual void OnNMSLobbyRegisterDone(unsigned id){}

        /// Lobby-Player-ID-Nachricht.
        virtual void OnNMSLobbyID(unsigned id, unsigned playerId){}
        /// Chat-Nachricht.
        virtual void OnNMSLobbyChat(unsigned id, const std::string& player, const std::string& text){}

        /// Ping-Nachricht.
        virtual void OnNMSLobbyPing(unsigned id){}
        /// Pong-Nachricht.
        virtual void OnNMSLobbyPong(unsigned id){}

        /// ServerList-Nachricht.
        virtual void OnNMSLobbyServerList(unsigned id){}
        virtual void OnNMSLobbyServerList(unsigned id, const LobbyServerList& list){}
        /// PlayerList-Nachricht.
        virtual void OnNMSLobbyPlayerList(unsigned id){}
        virtual void OnNMSLobbyPlayerList(unsigned id, const LobbyPlayerList& onlinePlayers, const LobbyPlayerList& ingamePlayers){}
        /// RankingList-Nachricht.
        virtual void OnNMSLobbyRankingList(unsigned id){}
        virtual void OnNMSLobbyRankingList(unsigned id, const LobbyPlayerList& list){}
        /// ServerInfo-Nachricht.
        virtual void OnNMSLobbyServerInfo(unsigned id, const unsigned& server){}
        virtual void OnNMSLobbyServerInfo(unsigned id, const LobbyServerInfo& info){}

        /// Lobby-Server-Add-Done-Nachricht.
        virtual void OnNMSLobbyServerAdd(unsigned id, const LobbyServerInfo& info){}
        /// Lobby-Server-Add-Failed-Nachricht.
        virtual void OnNMSLobbyServerAddFailed(unsigned id, const std::string& error){}
        /// Lobby-Server-Update-Player-Nachricht.
        virtual void OnNMSLobbyServerUpdatePlayer(unsigned id, const unsigned curplayer, const unsigned maxplayer){}
        /// Lobby-Server-Update-Map-Nachricht.
        virtual void OnNMSLobbyServerUpdateMap(unsigned id, const std::string& map){}
        /// Lobby-Server-Delete-Nachricht.
        virtual void OnNMSLobbyServerDelete(unsigned id){}
        /// Lobby-Server-Join-Nachricht.
        virtual void OnNMSLobbyServerJoin(unsigned id){}
        /// Lobby-Ranking-Info-Nachricht.
        virtual void OnNMSLobbyRankingInfo(unsigned id, const LobbyPlayerInfo& player){}
};

#endif //!LOBBYMESSAGEINTERFACE_H_INCLUDED
