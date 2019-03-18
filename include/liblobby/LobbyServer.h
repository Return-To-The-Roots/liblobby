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
#ifndef LOBBYSERVER_H_INCLUDED
#define LOBBYSERVER_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Header
#include "LobbyMessageInterface.h"

#include "LobbyMessages.h"
#include "LobbyPlayer.h"
#include <map>

/// Der LobbyServer
class LobbyServer : public Singleton<LobbyServer, SingletonPolicies::WithLongevity>, public LobbyMessageInterface
{
        using LobbyPlayerMap = std::map<unsigned, LobbyPlayer>;
        using LobbyPlayerMapIterator = std::map<unsigned, LobbyPlayer>::iterator;

    public:
        static const unsigned Longevity = 1;

        /// Konstruktor von @p LobbyServer.
        LobbyServer(void);
        /// Destruktor von @p LobbyServer.
        ~LobbyServer(void);

        /// Startet den Server.
        int Start(unsigned short port, std::string mysql_host, std::string mysql_user, std::string mysql_pass, std::string mysql_db);
        /// Nachrichtenschleife.
        int Run(void);
        /// Stoppt den Server.
        void Stop(void) { stop = true; }

    protected:
        /// prüft aktuelle Clients.
        bool CheckClientTimeouts();
        /// prüft auf neue Clients.
        bool CheckForNewClients();
        /// verarbeitet alle Nachrichten der Clients.
        bool ProcessMessages();

        /// verarbeitet die Todes-Nachricht eines Clients.
        virtual void OnNMSDead(unsigned id);

        /// Lobby-Login-Nachricht.
        virtual void OnNMSLobbyLogin(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& version);
        /// Lobby-Register-Nachricht.
        virtual void OnNMSLobbyRegister(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& email);
        /// Chat-Nachricht.
        virtual void OnNMSLobbyChat(unsigned id, const std::string& player, const std::string& text);
        /// Pong-Nachricht.
        virtual void OnNMSLobbyPong(unsigned id);

        /// ServerList-Nachricht.
        virtual void OnNMSLobbyServerList(unsigned id);
        /// PlayerList-Nachricht.
        virtual void OnNMSLobbyPlayerList(unsigned id);
        /// RankingList-Nachricht.
        virtual void OnNMSLobbyRankingList(unsigned id);
        /// ServerInfo-Nachricht.
        virtual void OnNMSLobbyServerInfo(unsigned id, const unsigned& server);

        /// Lobby-Server-Add-Done-Nachricht.
        virtual void OnNMSLobbyServerAdd(unsigned id, const LobbyServerInfo& info);
        /// Lobby-Server-Update-Player-Nachricht.
        virtual void OnNMSLobbyServerUpdatePlayer(unsigned id, const unsigned curplayer, const unsigned maxplayer);
        /// Lobby-Server-Update-Map-Nachricht.
        virtual void OnNMSLobbyServerUpdateMap(unsigned id, const std::string& map);
        /// Lobby-Server-Delete-Nachricht.
        virtual void OnNMSLobbyServerDelete(unsigned id);
        /// Lobby-Server-Join-Nachricht.
        virtual void OnNMSLobbyServerJoin(unsigned id);
        /// Lobby-Ranking-Info Nachricht.
        virtual void OnNMSLobbyRankingInfo(unsigned id, const LobbyPlayerInfo& player);

    private:
        /// schickt eine Nachricht an alle Clients.
        void SendToAll(const LobbyMessage& m);
        /// trennt die Verbindung zu einem Client.
        void Disconnect(LobbyPlayer& p);
        /// verschickt die Serverliste an einen Client.
        void SendServerList(unsigned client);
        /// verschickt die Playerliste an einen Client.
        void SendPlayerList(unsigned client);
        /// verschickt die Rankingliste an einen Client.
        void SendRankingList(unsigned client);

    private:
        /// Return the player with the given name or NULL if not found
        LobbyPlayer* GetPlayer(const std::string& name);
        /// Check if the users protocol version can be used. Return false if not and also notify and disconnect the user
        bool CheckProtocolVersion(unsigned userVersion, const std::string& userName, LobbyPlayer& player);

        bool stop;
        LobbyPlayerMap players;
        std::vector<unsigned> players_kill;

        Socket serverSock_;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define LOBBYSERVER LobbyServer::inst()

#endif // LOBBYSERVER_H_INCLUDED
