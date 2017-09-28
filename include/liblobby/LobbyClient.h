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
#ifndef LOBBYCLIENT_H_INCLUDED
#define LOBBYCLIENT_H_INCLUDED

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Eigene Header
#include "LobbyMessageInterface.h"
#include "LobbyPlayerList.h"
#include "LobbyServerInfo.h"
#include "LobbyServerList.h"
#include "libutil/MessageQueue.h"
#include "libutil/Singleton.h"
#include "libutil/Socket.h"
#include <string>

class LobbyInterface;
class LobbyPlayerInfo;

/// Lobby-Client-Klasse
class LobbyClient : public Singleton<LobbyClient, SingletonPolicies::WithLongevity>, public LobbyMessageInterface
{
public:
    static const unsigned Longevity = 10;

    LobbyClient();
    ~LobbyClient() override;

    /// setzt das Interface
    void SetInterface(LobbyInterface* parent) { this->listener = parent; }

    /// Hauptschleife.
    void Run();
    /// trennt die Verbindung mit dem LobbyServer.
    void Stop();

    /// versucht einen Login in den LobbyServer.
    bool Login(const std::string& server, const unsigned port, const std::string& user, const std::string& pass, const bool use_ipv6);
    ///  versucht einen Nick auf dem LobbyServer zu registrieren.
    bool Register(const std::string& server, const unsigned port, const std::string& user, const std::string& pass,
                  const std::string& email, const bool use_ipv6);

    /// schickt einen Request für die Server-Liste.
    void SendServerListRequest();
    /// schickt einen Request für die Player-Liste.
    void SendPlayerListRequest();
    /// schickt einen Request für die Top10-Liste.
    void SendRankingListRequest();
    /// schickt einen Request für ein Serverinfo.
    void SendServerInfoRequest(unsigned id);
    /// schickt einen Request um die Punkte eines bestimmten Spielers auszulesen.
    void SendRankingInfoRequest(const std::string& name);

    /// schickt einen Request für den Serverjoin.
    void SendServerJoinRequest();
    /// Notify that we left a server
    void SendLeaveServer();

    /// verschickt eine Chatnachricht.
    void SendChat(const std::string& text);

    /// fügt einen Server zur Lobby hinzu.
    void AddServer(const std::string& name, const std::string& programVersion, const std::string& map, bool has_password,
                   unsigned short port);
    /// entfernt den einem zugehörigen Server von der Lobby.
    void DeleteServer();
    /// aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
    void UpdateServerMap(const std::string& map);
    /// aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
    void UpdateServerPlayerCount(unsigned curplayer, unsigned maxplayer);

    /// liefert die Serverliste.
    const LobbyServerList& GetServerList() const { return serverList; }
    /// liefert die Spielerliste.
    const LobbyPlayerList& GetPlayerList() const { return playerList; };
    /// liefert die Rankingliste.
    const LobbyPlayerList& GetRankingList() const { return rankingList; }
    /// liefert Informationen über einen Server
    const LobbyServerInfo& GetServerInfo() const { return serverInfo; };
    const std::string GetUser() const { return userdata.user; };

    /// sind wir eingeloggt?
    bool IsLoggedIn() const { return (state == CS_LOBBY || state == CS_INGAME); }
    bool IsIngame() const { return state == CS_INGAME; }

    /// Lobby-Login-Error-Nachricht.
    void OnNMSLobbyLoginError(unsigned id, const std::string& error) override;
    /// Lobby-Logged-In-Nachricht.
    void OnNMSLobbyLoginDone(unsigned id, const std::string& email) override;

    /// Lobby-Register-Error-Nachricht.
    void OnNMSLobbyRegisterError(unsigned id, const std::string& error) override;
    /// Lobby-Register-Done-Nachricht.
    void OnNMSLobbyRegisterDone(unsigned id) override;

    /// Lobby-Player-ID-Nachricht.
    void OnNMSLobbyID(unsigned id, unsigned playerId) override;
    /// Chat-Nachricht.
    void OnNMSLobbyChat(unsigned id, const std::string& player, const std::string& text) override;

    /// Ping-Nachricht.
    void OnNMSLobbyPing(unsigned id) override;

    /// ServerList-Nachricht.
    void OnNMSLobbyServerList(unsigned id, const LobbyServerList& list) override;
    /// PlayerList-Nachricht.
    void OnNMSLobbyPlayerList(unsigned id, const LobbyPlayerList& onlinePlayers, const LobbyPlayerList& ingamePlayers) override;
    /// RankingList-Nachricht.
    void OnNMSLobbyRankingList(unsigned id, const LobbyPlayerList& list) override;
    /// ServerInfo-Nachricht.
    void OnNMSLobbyServerInfo(unsigned id, const LobbyServerInfo& info) override;

    /// Lobby-Server-Add-Done-Nachricht.
    void OnNMSLobbyServerAdd(unsigned id, const LobbyServerInfo& info) override;
    /// Lobby-Server-Add-Failed-Nachricht.
    void OnNMSLobbyServerAddFailed(unsigned id, const std::string& error) override;

    /// Lobby-Ranking-Info Nachricht.
    void OnNMSLobbyRankingInfo(unsigned id, const LobbyPlayerInfo& player) override;

    /// Dead-Nachricht.
    virtual void OnNMSDeadMsg(unsigned id);

protected:
    /// verbindet mit dem LobbyServer.
    bool Connect(const std::string& server, const unsigned port, const bool use_ipv6);
    /// Server verloren.
    void ServerLost(bool notifyListener = true);

public:
    bool receivedNewServerList;
    bool receivedNewServerInfo;
    bool receivedNewRankingList;
    bool receivedNewPlayerList;

private:
    LobbyInterface* listener;

    MessageQueue recv_queue;
    MessageQueue send_queue;

    enum ClientState
    {
        CS_STOPPED = 0,
        CS_CONNECT,
        CS_LOBBY,
        CS_INGAME
    } state;

    enum ClientTodo
    {
        TD_NOTHING = 0,
        TD_LOGIN,
        TD_REGISTER
    } todoAfterConnect;

    struct UserData
    {
        std::string user;
        std::string pass;
        std::string email;
        std::string programVersion;
    } userdata;

    Socket socket;

    LobbyServerList serverList;
    LobbyPlayerList playerList;
    LobbyPlayerList rankingList;
    LobbyServerInfo serverInfo;
    bool isHost;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define LOBBYCLIENT LobbyClient::inst()

#endif // LOBBYCLIENT_H_INCLUDED
