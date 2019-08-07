//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyClient_h
#define liblobby_include_liblobby_LobbyClient_h

#include "LobbyMessageInterface.h"
#include "LobbyPlayerList.h"
#include "LobbyServerInfo.h"
#include "LobbyServerList.h"

#include <libutil/MessageQueue.h>
#include <libutil/Singleton.h>
#include <libutil/Socket.h>

#include <string>

class LobbyInterface;
class LobbyPlayerInfo;

/// Lobby-Client-Klasse
class LobbyClient : public Singleton<LobbyClient, SingletonPolicies::WithLongevity>, public LobbyMessageInterface
{
public:
    static const unsigned Longevity = 10;

    LobbyClient();
    ~LobbyClient();

    void SetProgramVersion(const std::string& programVersion) { this->programVersion = programVersion; }
    /// setzt das Interface
    void AddListener(LobbyInterface* listener);
    void RemoveListener(LobbyInterface* listener);

    /// Hauptschleife.
    void Run();
    /// trennt die Verbindung mit dem LobbyServer.
    void Stop();

    /// versucht einen Login in den LobbyServer.
    bool Login(const std::string& server, unsigned port, const std::string& user, const std::string& pass, bool use_ipv6);

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
    void AddServer(const std::string& name, const std::string& map, bool has_password, unsigned short port);
    /// entfernt den einem zugehörigen Server von der Lobby.
    void DeleteServer();
    /// aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
    void UpdateServerMap(const std::string& map);
    /// aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
    void UpdateServerNumPlayers(unsigned curplayer, unsigned maxplayer);

    /// liefert die Serverliste.
    const LobbyServerList& GetServerList() const { return serverList; }
    /// liefert die Spielerliste.
    const LobbyPlayerList& GetPlayerList() const { return playerList; };
    /// liefert die Rankingliste.
    const LobbyPlayerList& GetRankingList() const { return rankingList; }
    /// liefert Informationen über einen Server
    const LobbyServerInfo& GetServerInfo() const { return serverInfo; };
    const std::string& GetUser() const { return userdata.user; };

    /// sind wir eingeloggt?
    bool IsLoggedIn() const { return (state == CS_LOBBY || state == CS_INGAME); }
    bool IsIngame() const { return state == CS_INGAME; }

    /// Lobby-Login-Error-Nachricht.
    bool OnNMSLobbyLoginError(unsigned id, const std::string& error) override;
    /// Lobby-Logged-In-Nachricht.
    bool OnNMSLobbyLoginDone(unsigned id, const std::string& email) override;

    /// Lobby-Player-ID-Nachricht.
    bool OnNMSLobbyID(unsigned id, unsigned playerId) override;
    /// Chat-Nachricht.
    bool OnNMSLobbyChat(unsigned id, const std::string& player, const std::string& text) override;

    /// Ping-Nachricht.
    bool OnNMSLobbyPing(unsigned id) override;

    /// ServerList-Nachricht.
    bool OnNMSLobbyServerList(unsigned) override { return false; }
    bool OnNMSLobbyServerList(unsigned id, const LobbyServerList& list) override;
    /// PlayerList-Nachricht.
    bool OnNMSLobbyPlayerList(unsigned) override { return false; }
    bool OnNMSLobbyPlayerList(unsigned id, const LobbyPlayerList& onlinePlayers, const LobbyPlayerList& ingamePlayers) override;
    /// RankingList-Nachricht.
    bool OnNMSLobbyRankingList(unsigned) override { return false; }
    bool OnNMSLobbyRankingList(unsigned id, const LobbyPlayerList& list) override;
    /// ServerInfo-Nachricht.
    bool OnNMSLobbyServerInfo(unsigned, const unsigned&) override { return false; }
    bool OnNMSLobbyServerInfo(unsigned id, const LobbyServerInfo& info) override;

    /// Lobby-Server-Add-Done-Nachricht.
    bool OnNMSLobbyServerAdd(unsigned id, const LobbyServerInfo& info) override;
    /// Lobby-Server-Add-Failed-Nachricht.
    bool OnNMSLobbyServerAddFailed(unsigned id, const std::string& error) override;

    /// Lobby-Ranking-Info Nachricht.
    bool OnNMSLobbyRankingInfo(unsigned id, const LobbyPlayerInfo& player) override;

    /// Dead-Nachricht.
    virtual bool OnNMSDeadMsg(unsigned id);

protected:
    /// verbindet mit dem LobbyServer.
    bool Connect(const std::string& server, unsigned port, bool use_ipv6);
    /// Server verloren.
    void ServerLost(bool notifyParent = true);

private:
    std::string programVersion;
    std::vector<LobbyInterface*> listeners;

    MessageQueue recv_queue;
    MessageQueue send_queue;

    enum ClientState
    {
        CS_STOPPED = 0,
        CS_CONNECT,
        CS_LOBBY,
        CS_INGAME
    } state;

    struct UserData
    {
        std::string user;
        std::string pass;
        std::string email;
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

#endif // !liblobby_include_liblobby_LobbyClient_h
