// $Id: LobbyClient.h 6582 2010-07-16 11:23:35Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#include "Singleton.h"

#include "LobbyInterface.h"

#include "LobbyMessage.h"
#include "LobbyMessageInterface.h"

#include "LobbyServerList.h"
#include "LobbyServerInfo.h"
#include "LobbyPlayerList.h"

#include "MessageQueue.h"

/// Lobby-Client-Klasse
class LobbyClient : public Singleton<LobbyClient>, public LobbyMessageInterface
{
public:
	/// Konstruktor von @p LobbyClient.
	LobbyClient(void);
	/// Destruktor von @p LobbyClient.
	~LobbyClient(void);

	/// setzt das Interface
	void SetInterface(LobbyInterface *parent) { this->parent = parent; }

	/// Hauptschleife.
	void Run();
	/// trennt die Verbindung mit dem LobbyServer.
	void Stop(void);

	/// versucht einen Login in den LobbyServer.
	bool Login(const std::string &server, const unsigned int port, const std::string &user, const std::string &pass, const bool use_ipv6);
	///  versucht einen Nick auf dem LobbyServer zu registrieren.
	bool Register(const std::string &server, const unsigned int port, const std::string &user, const std::string &pass, const std::string &email, const bool use_ipv6);

	/// schickt einen Request für die Server-Liste.
	void SendServerListRequest(void);
	/// schickt einen Request für die Player-Liste.
	void SendPlayerListRequest(void);
	/// schickt einen Request für die Top10-Liste.
	void SendRankingListRequest(void);
	/// schickt einen Request für ein Serverinfo.
	void SendServerInfoRequest(unsigned int id);
	/// schickt einen Request für den Serverjoin.
	void SendServerJoinRequest(void);
	/// schickt einen Request um die Punkte eines bestimmten Spielers auszulesen.
	void SendRankingInfoRequest(const std::string name);

	/// verschickt eine Chatnachricht.
	void SendChat(std::string text);

	/// fügt einen Server zur Lobby hinzu.
	void AddServer(std::string name, std::string map, bool has_password, unsigned short port);
	/// entfernt den einem zugehörigen Server von der Lobby.
	void DeleteServer();
	/// aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
	void UpdateServer(std::string map);
	/// aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
	void UpdateServerPlayerCount(unsigned int curplayer, unsigned int maxplayer);

	/// liefert die Serverliste.
	const LobbyServerList *GetServerList(void) { return &serverlist; }
	/// liefert die Spielerliste.
	const LobbyPlayerList *GetPlayerList(void) { return &playerlist; };
	/// liefert die Rankingliste.
	const LobbyPlayerList *GetRankingList(void) { return &rankinglist; }
	/// liefert Informationen über einen Server
	const LobbyServerInfo *GetServerInfo(void) { return &serverinfo; };

	/// sind wir eingeloggt?
	bool LoggedIn() { return (state == CS_LOBBY); }

	/// Lobby-Login-Error-Nachricht.
	virtual void OnNMSLobbyLoginError(unsigned int id, const std::string &error);
	/// Lobby-Logged-In-Nachricht.
	virtual void OnNMSLobbyLoginDone(unsigned int id, const std::string &email);

	/// Lobby-Register-Error-Nachricht.
	virtual void OnNMSLobbyRegisterError(unsigned int id, const std::string &error);
	/// Lobby-Register-Done-Nachricht.
	virtual void OnNMSLobbyRegisterDone(unsigned int id);

	/// Lobby-Player-ID-Nachricht.
	virtual void OnNMSLobbyID(unsigned int id, const unsigned int &playerid);
	/// Chat-Nachricht.
	virtual void OnNMSLobbyChat(unsigned int id, const std::string &player, const std::string &text);

	/// Ping-Nachricht.
	virtual void OnNMSLobbyPing(unsigned int id);

	/// ServerList-Nachricht.
	virtual void OnNMSLobbyServerList(unsigned int id, const LobbyServerList &list);
	/// PlayerList-Nachricht.
	virtual void OnNMSLobbyPlayerList(unsigned int id, const LobbyPlayerList &list);
	/// RankingList-Nachricht.
	virtual void OnNMSLobbyRankingList(unsigned int id, const LobbyPlayerList &list);
	/// ServerInfo-Nachricht.
	virtual void OnNMSLobbyServerInfo(unsigned int id, const LobbyServerInfo &info);

	/// Lobby-Server-Add-Done-Nachricht.
	virtual void OnNMSLobbyServerAdd(unsigned int id, const LobbyServerInfo &info);
	/// Lobby-Server-Add-Failed-Nachricht.
	virtual void OnNMSLobbyServerAddFailed(unsigned int id, const std::string &error);

	/// Lobby-Ranking-Info Nachricht.
	virtual void OnNMSLobbyRankingInfo(unsigned int id, const LobbyPlayerInfo &player);

	/// Dead-Nachricht.
	virtual void OnNMSDeadMsg(unsigned int id);

protected:
	/// verbindet mit dem LobbyServer.
	bool Connect(const std::string &server, const unsigned int port, const bool use_ipv6);
	/// Server verloren.
	void ServerLost(bool message = true);

public:
	bool refreshserverlist;
	bool refreshserverinfo;
	bool refreshrankinglist;
	bool refreshplayerlist;

private:
	LobbyInterface *parent;

	MessageQueue recv_queue;
	MessageQueue send_queue;

	enum ClientState
	{
		CS_STOPPED = 0,
		CS_CONNECT,
		CS_LOBBY
	} state;

	enum ClientTodo
	{
		TD_NOTHING = 0,
		TD_LOGIN,
		TD_REGISTER
	} todo;

	struct UserData
	{
		std::string user;
		std::string pass;
		std::string email;
	} userdata;

	std::string error;

	Socket socket;

	LobbyServerList	serverlist;
	LobbyPlayerList playerlist;
	LobbyPlayerList rankinglist;
	LobbyServerInfo serverinfo;
	LobbyServerInfo server;
};

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#define LOBBYCLIENT LobbyClient::inst()

#endif // LOBBYCLIENT_H_INCLUDED
