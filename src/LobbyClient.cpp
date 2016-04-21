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

#include "main.h" // IWYU pragma: keep
#include "LobbyClient.h"
#include "Messages.h"
#include "LobbyInterface.h"
#include "LobbyMessage.h"
#include "LobbyMessages.h"
#include "Log.h"
#include "Message.h"
#include "SocketSet.h"
#include "mygettext.h"
#include <build_version.h>

#include <cstddef>

class LobbyPlayerInfo;

LobbyClient::LobbyClient()
    : refreshserverlist(false), refreshserverinfo(false), refreshrankinglist(false), refreshplayerlist(false),
      parent(NULL), recv_queue(&LobbyMessage::create_lobby), send_queue(&LobbyMessage::create_lobby),
      state(CS_STOPPED), todo(TD_NOTHING)
{
}

LobbyClient::~LobbyClient()
{
    Stop();
}

/**
 *  Hauptschleife.
 */
void LobbyClient::Run()
{
    if(state == CS_STOPPED)
        return;

    SocketSet set;

    // erstmal auf Daten überprüfen
    set.Clear();

    // zum set hinzufügen
    set.Add(socket);
    if(set.Select(0, 0) > 0)
    {
        // nachricht empfangen
        if(!recv_queue.recv(socket))
        {
            LOG.lprintf("Receiving Message from server failed\n");
            ServerLost();
            return;
        }
    }

    // nun auf Fehler prüfen
    set.Clear();

    // zum set hinzufügen
    set.Add(socket);

    // auf fehler prüfen
    if(set.Select(0, 2) > 0)
    {
        if(set.InSet(socket))
        {
            // Server ist weg
            LOG.lprintf("Error on socket to server\n");
            ServerLost();
            return;
        }
    }

    // maximal 10 Pakete verschicken
    if(!send_queue.send(socket, 10))
    {
        ServerLost();
        return;
    }

    // recv-queue abarbeiten
    while(recv_queue.count() > 0)
    {
        recv_queue.front()->run(this, 0xFFFFFFFF);
        recv_queue.pop();
    }
}

/**
 *  trennt die Verbindung mit dem LobbyServer.
 */
void LobbyClient::Stop()
{
    if(state != CS_CONNECT)
    {
        send_queue.push(new Message_Dead());
        send_queue.flush(socket);
    }

    // Verbindung trennen
    socket.Close();

    // Queues leeren
    recv_queue.clear();
    send_queue.clear();

    state = CS_STOPPED;
    todo = TD_NOTHING;

    refreshserverlist = false;
    refreshplayerlist = false;
    refreshrankinglist = false;
    refreshserverinfo = false;
}

/**
 *  versucht einen Login in den LobbyServer.
 *
 *  @param[in] user Benutzername
 *  @param[in] pass Passwort
 */
bool LobbyClient::Login(const std::string& server, const unsigned int port, const std::string& user, const std::string& pass, const bool use_ipv6)
{
    // aufräumen
    Stop();

    todo = TD_LOGIN;

    userdata.user = user;
    userdata.pass =  pass;

    // verbinden
    return Connect( server, port, use_ipv6 );
}

/**
 *  versucht einen Nick auf dem LobbyServer zu registrieren.
 *
 *  @param[in] user Benutzername
 *  @param[in] pass Passwort
 */
bool LobbyClient::Register(const std::string& server, const unsigned int port, const std::string& user, const std::string& pass, const std::string& email, const bool use_ipv6)
{
    // aufräumen
    Stop();

    todo = TD_REGISTER;

    userdata.user = user;
    userdata.pass = pass;
    userdata.email = email;

    // verbinden
    return Connect( server, port, use_ipv6 );
}

/**
 *  schickt einen Request für die Server-Liste.
 */
void LobbyClient::SendServerListRequest()
{
    send_queue.push(new LobbyMessage_ServerList(1));
}

/**
 *  schickt einen Request für die Player-Liste.
 */
void LobbyClient::SendPlayerListRequest()
{
    send_queue.push(new LobbyMessage_PlayerList(1));
}

/**
 *  schickt einen Request für die Top10-Liste.
 */
void LobbyClient::SendRankingListRequest()
{
    send_queue.push(new LobbyMessage_RankingList(1));
}

/**
 *  schickt einen Request für ein Serverinfo.
 *
 *  @param[in] id Server-ID
 */
void LobbyClient::SendServerInfoRequest(unsigned int id)
{
    if(id == 0)
        return;

    send_queue.push(new LobbyMessage_ServerInfo(id));
}

/**
 *  schickt einen Request für den Serverjoin.
 */
void LobbyClient::SendServerJoinRequest()
{
    send_queue.push(new LobbyMessage_Server_Join());
}

/**
 *  schickt einen Request um die Punkte eines bestimmten Spielers auszulesen.
 */
void LobbyClient::SendRankingInfoRequest(const std::string& name)
{
    send_queue.push(new LobbyMessage_Lobby_Ranking_Info(name));
}

/**
 *  verschickt eine Chatnachricht.
 *
 *  @param[in] text Der Text
 */
void LobbyClient::SendChat(const std::string& text)
{
    send_queue.push(new LobbyMessage_Chat(text));
}

/**
 *  fügt einen Server zur Lobby hinzu.
 *
 *  @param[in] name Name des Servers.
 */
void LobbyClient::AddServer(const std::string& name, const std::string& map, bool has_password, unsigned short port)
{
    server_.clear();
    server_.setName(name);
    server_.setVersion(GetWindowVersion());
    server_.setPort(port);
    server_.setMap(map);
    server_.setPassword(has_password);

    send_queue.push(new LobbyMessage_Server_Add(server_));

    LOG.lprintf("LobbyClient: GameServer %s wird erstellt ...\n", name.c_str());
}

/**
 *  entfernt den einem zugehörigen Server von der Lobby.
 */
void LobbyClient::DeleteServer()
{
    send_queue.push(new LobbyMessage_Server_Delete());

    server_.clear();
}

/**
 *  aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] map Kartenname
 */
void LobbyClient::UpdateServer(const std::string& map)
{
    server_.setMap(map);

    send_queue.push(new LobbyMessage_Server_Update_Map(map));
}

/**
 *  aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] curplayer Aktuelle Anzahl Spieler
 *  @param[in] maxplayer Maximale Anzahl Spieler
 */
void LobbyClient::UpdateServerPlayerCount(unsigned int curplayer, unsigned int maxplayer)
{
    server_.setCurPlayers(curplayer);
    server_.setMaxPlayers(maxplayer);

    send_queue.push(new LobbyMessage_Server_Update_Player(curplayer, maxplayer));
}

/**
 *  verbindet mit dem LobbyServer.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool LobbyClient::Connect(const std::string& server, const unsigned int port, const bool use_ipv6)
{
    // Verbinden
    if(!socket.Connect(server, port, use_ipv6))
    {
        ServerLost();
        return false;
    }

    state = CS_CONNECT;

    if(parent)
        parent->LC_Status_Waiting();

    return true;
}

/**
 *  Ping-Nachricht.
 */
void LobbyClient::OnNMSLobbyPing(unsigned int  /*id*/)
{
    send_queue.push(new LobbyMessage_Pong(1));
}

/**
 *  Lobby-Player-ID-Nachricht.
 *
 *  @param[in] playerid Die empfangene Spieler-ID
 */
void LobbyClient::OnNMSLobbyID(unsigned int  /*id*/, const unsigned int& playerid)
{
    if(playerid == 0xFFFFFFFF)
    {
        if(parent)
            parent->LC_Status_Error(_("This Server is full!"));
        ServerLost(false);

        return;
    }

    switch(todo)
    {
        case TD_LOGIN:
        {
            send_queue.push(new LobbyMessage_Login(userdata.user, userdata.pass, GetWindowVersion()));
        } break;
        case TD_REGISTER:
        {
            send_queue.push(new LobbyMessage_Register(userdata.user, userdata.pass, userdata.email));
        } break;
        default:
        {
            ServerLost();
        } break;
    }
}

/**
 *  Lobby-Login-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyLoginError(unsigned int  /*id*/, const std::string& error)
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);

    ServerLost(false);
}

/**
 *  Lobby-Logged-In-Nachricht.
 *
 *  @param[in] email Die empfangene Emailadresse des Accounts
 */
void LobbyClient::OnNMSLobbyLoginDone(unsigned int  /*id*/, const std::string& email)
{
    userdata.email = email;
    if(parent)
        parent->LC_LoggedIn(userdata.email);

    state = CS_LOBBY;
}

/**
 *  Lobby-Register-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyRegisterError(unsigned int  /*id*/, const std::string& error) //-V524
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);

    ServerLost(false);
}

/**
 *  Lobby-Register-Done-Nachricht.
 */
void LobbyClient::OnNMSLobbyRegisterDone(unsigned int  /*id*/)
{
    if(parent)
        parent->LC_Registered();

    Stop();
}

/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Die empfangene Serverliste
 */
void LobbyClient::OnNMSLobbyServerList(unsigned int  /*id*/, const LobbyServerList& list)
{
    serverlist = list;

    refreshserverlist = true;
}

/**
 *  PlayerList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste
 */
void LobbyClient::OnNMSLobbyPlayerList(unsigned int  /*id*/, const LobbyPlayerList& list)
{
    playerlist = list;

    refreshplayerlist = true;
}

/**
 *  RankingList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste (Top 10)
 */
void LobbyClient::OnNMSLobbyRankingList(unsigned int  /*id*/, const LobbyPlayerList& list)
{
    rankinglist = list;

    refreshrankinglist = true;
}

/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Das empfangene ServerInfo
 */
void LobbyClient::OnNMSLobbyServerInfo(unsigned int  /*id*/, const LobbyServerInfo& info)
{
    serverinfo = info;

    refreshserverinfo = true;
}

/**
 *  Chat-Nachricht.
 *
 *  @param[in] player Der empfangene Spielername
 *  @param[in] text   Der empfangene Text
 */
void LobbyClient::OnNMSLobbyChat(unsigned int  /*id*/, const std::string& player, const std::string& text)
{
    if(parent)
        parent->LC_Chat(player, text);
}

/**
 *  Lobby-Server-Add-Failed-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyServerAddFailed(unsigned int  /*id*/, const std::string& error)
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);
}

/**
 *  Lobby-Server-Add-Done-Nachricht.
 *
 *  @param[in] info Das empfangene ServerInfo
 */
void LobbyClient::OnNMSLobbyServerAdd(unsigned int  /*id*/, const LobbyServerInfo& info)
{
    server_ = info;

    LOG.lprintf("LobbyClient: GameServer %s erfolgreich erstellt!\n", server_.getName().c_str());

    // Server kann jetzt gestartet werden
    if(parent)
        parent->LC_Created();
}

/**
 *  Lobby-Ranking-Info Nachricht.
 *
 *  @param[in] email Die empfangene PlayerInfo des angefragten Spielers.
 */
void LobbyClient::OnNMSLobbyRankingInfo(unsigned int  /*id*/, const LobbyPlayerInfo& player)
{
    if(parent)
        parent->LC_RankingInfo(player);
}

/**
 *  Dead-Nachricht.
 */
void LobbyClient::OnNMSDeadMsg(unsigned int  /*id*/)
{
    ServerLost();
}

/**
 *  Server verloren.
 */
void LobbyClient::ServerLost(bool message)
{
    if(state != CS_STOPPED)
        LOG.lprintf("lobby client forced to stop\n");

    Stop();

    if(parent && message)
        parent->LC_Status_ConnectionLost();
}
