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

/// Der LobbyServer
class LobbyServer : public Singleton<LobbyServer>, public LobbyMessageInterface
{
        typedef std::map<unsigned int, LobbyPlayer> LobbyPlayerMap;
        typedef std::map<unsigned int, LobbyPlayer>::iterator LobbyPlayerMapIterator;

    public:
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
        bool Test();
        /// prüft auf neue Clients.
        bool Await();
        /// verarbeitet alle Nachrichten der Clients.
        bool Forward();

        /// verarbeitet die Todes-Nachricht eines Clients.
        virtual void OnNMSDead(unsigned int id);

        /// Lobby-Login-Nachricht.
        virtual void OnNMSLobbyLogin(unsigned int id, const unsigned int revision, const std::string& user, const std::string& pass, const std::string& version);
        /// Lobby-Register-Nachricht.
        virtual void OnNMSLobbyRegister(unsigned int id, const unsigned int revision, const std::string& user, const std::string& pass, const std::string& email);
        /// Chat-Nachricht.
        virtual void OnNMSLobbyChat(unsigned int id, const std::string& player, const std::string& text);
        /// Pong-Nachricht.
        virtual void OnNMSLobbyPong(unsigned int id);

        /// ServerList-Nachricht.
        virtual void OnNMSLobbyServerList(unsigned int id);
        /// PlayerList-Nachricht.
        virtual void OnNMSLobbyPlayerList(unsigned int id);
        /// RankingList-Nachricht.
        virtual void OnNMSLobbyRankingList(unsigned int id);
        /// ServerInfo-Nachricht.
        virtual void OnNMSLobbyServerInfo(unsigned int id, const unsigned int& server);

        /// Lobby-Server-Add-Done-Nachricht.
        virtual void OnNMSLobbyServerAdd(unsigned int id, const LobbyServerInfo& info);
        /// Lobby-Server-Update-Player-Nachricht.
        virtual void OnNMSLobbyServerUpdatePlayer(unsigned int id, const unsigned int curplayer, const unsigned int maxplayer);
        /// Lobby-Server-Update-Map-Nachricht.
        virtual void OnNMSLobbyServerUpdateMap(unsigned int id, const std::string& map);
        /// Lobby-Server-Delete-Nachricht.
        virtual void OnNMSLobbyServerDelete(unsigned int id);
        /// Lobby-Server-Join-Nachricht.
        virtual void OnNMSLobbyServerJoin(unsigned int id);
        /// Lobby-Ranking-Info Nachricht.
        virtual void OnNMSLobbyRankingInfo(unsigned int id, const LobbyPlayerInfo& player);

    private:
        /// schickt eine Nachricht an alle Clients.
        void SendToAll(const LobbyMessage* m);
        /// trennt die Verbindung zu einem Client.
        void Disconnect(LobbyPlayer& p);
        /// verschickt die Serverliste an einen Client.
        void SendServerList(unsigned int client);
        /// verschickt die Playerliste an einen Client.
        void SendPlayerList(unsigned int client);
        /// verschickt die Rankingliste an einen Client.
        void SendRankingList(unsigned int client);

    private:
        bool stop;
        LobbyPlayerMap players;
        std::vector<unsigned int> players_kill;

        Socket server;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define LOBBYSERVER LobbyServer::inst()

#endif // LOBBYSERVER_H_INCLUDED
