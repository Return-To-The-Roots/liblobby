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

#include "libLobbyDefines.h" // IWYU pragma: keep
#include "LobbyClient.h"
#include "libutil/src/Messages.h"
#include "LobbyInterface.h"
#include "LobbyMessage.h"
#include "LobbyMessages.h"
#include "libutil/src/Log.h"
#include "libutil/src/Message.h"
#include "libutil/src/SocketSet.h"
#include "mygettext/src/mygettext.h"
#include <build_version.h>

#include <cstddef>

class LobbyPlayerInfo;

LobbyClient::LobbyClient()
    : receivedNewServerList(false), receivedNewServerInfo(false), receivedNewRankingList(false), receivedNewPlayerList(false),
      listener(NULL), recv_queue(&LobbyMessage::create_lobby), send_queue(&LobbyMessage::create_lobby),
      state(CS_STOPPED), todoAfterConnect(TD_NOTHING), isHost(false)
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
            LOG.write("Receiving Message from server failed\n");
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
            LOG.write("Error on socket to server\n");
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
    todoAfterConnect = TD_NOTHING;

    receivedNewServerList = false;
    receivedNewPlayerList = false;
    receivedNewRankingList = false;
    receivedNewServerInfo = false;
    isHost = false;
}

/**
 *  versucht einen Login in den LobbyServer.
 *
 *  @param[in] user Benutzername
 *  @param[in] pass Passwort
 */
bool LobbyClient::Login(const std::string& server, const unsigned port, const std::string& user, const std::string& pass, const bool use_ipv6)
{
    // aufräumen
    Stop();

    todoAfterConnect = TD_LOGIN;

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
bool LobbyClient::Register(const std::string& server, const unsigned port, const std::string& user, const std::string& pass, const std::string& email, const bool use_ipv6)
{
    // aufräumen
    Stop();

    todoAfterConnect = TD_REGISTER;

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
    assert(state == CS_LOBBY);
    send_queue.push(new LobbyMessage_ServerList(1));
}

/**
 *  schickt einen Request für die Player-Liste.
 */
void LobbyClient::SendPlayerListRequest()
{
    assert(state == CS_LOBBY);
    send_queue.push(new LobbyMessage_PlayerList(1));
}

/**
 *  schickt einen Request für die Top10-Liste.
 */
void LobbyClient::SendRankingListRequest()
{
    assert(state == CS_LOBBY);
    send_queue.push(new LobbyMessage_RankingList(1));
}

/**
 *  schickt einen Request für ein Serverinfo.
 *
 *  @param[in] id Server-ID
 */
void LobbyClient::SendServerInfoRequest(unsigned id)
{
    assert(state == CS_LOBBY);
    if(id == 0)
        return;

    send_queue.push(new LobbyMessage_ServerInfo(id));
}

/**
 *  schickt einen Request für den Serverjoin.
 */
void LobbyClient::SendServerJoinRequest()
{
    if(state == CS_INGAME)
    {
        // Currently host gets autojoined. TODO: Change protocol
        assert(isHost);
        return;
    }
    assert(state == CS_LOBBY);
    send_queue.push(new LobbyMessage_Server_Join());
    state = CS_INGAME;
}

void LobbyClient::SendLeaveServer()
{
    // Don't send if we are not ingame or we will be kicked
    if(state == CS_INGAME){
        // We don't have a dedicated message, but sending a delete request will also reset our state
        send_queue.push(new LobbyMessage_Server_Delete());
        state = CS_LOBBY;
    }
}

/**
 *  schickt einen Request um die Punkte eines bestimmten Spielers auszulesen.
 */
void LobbyClient::SendRankingInfoRequest(const std::string& name)
{
    assert(IsLoggedIn());
    send_queue.push(new LobbyMessage_Lobby_Ranking_Info(name));
}

/**
 *  verschickt eine Chatnachricht.
 *
 *  @param[in] text Der Text
 */
void LobbyClient::SendChat(const std::string& text)
{
    assert(state == CS_LOBBY || state == CS_INGAME);
    send_queue.push(new LobbyMessage_Chat(text));
}

/**
 *  fügt einen Server zur Lobby hinzu.
 *
 *  @param[in] name Name des Servers.
 */
void LobbyClient::AddServer(const std::string& name, const std::string& map, bool has_password, unsigned short port)
{
    assert(state == CS_LOBBY);
    LobbyServerInfo server;
    server.setName(name);
    server.setVersion(GetWindowVersion());
    server.setPort(port);
    server.setMap(map);
    server.setPassword(has_password);

    send_queue.push(new LobbyMessage_Server_Add(server));

    LOG.write("LobbyClient: GameServer %s wird erstellt ...\n") % name;
}

/**
 *  entfernt den einem zugehörigen Server von der Lobby.
 */
void LobbyClient::DeleteServer()
{
    if(state == CS_INGAME && isHost){
        send_queue.push(new LobbyMessage_Server_Delete());
        isHost = false;
        // TODO: current protocol says we are not ingame anymore
        state = CS_LOBBY;
    }
}

/**
 *  aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] map Kartenname
 */
void LobbyClient::UpdateServerMap(const std::string& map)
{
    assert(state == CS_INGAME && isHost);
    send_queue.push(new LobbyMessage_Server_Update_Map(map));
}

/**
 *  aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] curplayer Aktuelle Anzahl Spieler
 *  @param[in] maxplayer Maximale Anzahl Spieler
 */
void LobbyClient::UpdateServerPlayerCount(unsigned curplayer, unsigned maxplayer)
{
    assert(state == CS_INGAME && isHost);
    send_queue.push(new LobbyMessage_Server_Update_Player(curplayer, maxplayer));
}

/**
 *  verbindet mit dem LobbyServer.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 */
bool LobbyClient::Connect(const std::string& server, const unsigned port, const bool use_ipv6)
{
    assert(state == CS_STOPPED);
    // Verbinden
    if(!socket.Connect(server, port, use_ipv6))
    {
        ServerLost();
        return false;
    }

    state = CS_CONNECT;

    if(listener)
        listener->LC_Status_Waiting();

    return true;
}

/**
 *  Ping-Nachricht.
 */
void LobbyClient::OnNMSLobbyPing(unsigned  /*id*/)
{
    send_queue.push(new LobbyMessage_Pong(1));
}

/**
 *  Lobby-Player-ID-Nachricht.
 *
 *  @param[in] playerId Die empfangene Spieler-ID
 */
void LobbyClient::OnNMSLobbyID(unsigned  /*id*/, unsigned playerId)
{
    if(playerId == 0xFFFFFFFF)
    {
        if(listener)
            listener->LC_Status_Error(_("This Server is full!"));
        ServerLost(false);

        return;
    }

    switch(todoAfterConnect)
    {
        case TD_LOGIN:
            send_queue.push(new LobbyMessage_Login(userdata.user, userdata.pass, GetWindowVersion()));
            break;
        case TD_REGISTER:
            send_queue.push(new LobbyMessage_Register(userdata.user, userdata.pass, userdata.email));
            break;
        default:
            ServerLost();
            break;
    }
}

/**
 *  Lobby-Login-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyLoginError(unsigned  /*id*/, const std::string& error)
{
    if(listener)
        listener->LC_Status_Error(error);

    ServerLost(false);
}

/**
 *  Lobby-Logged-In-Nachricht.
 *
 *  @param[in] email Die empfangene Emailadresse des Accounts
 */
void LobbyClient::OnNMSLobbyLoginDone(unsigned  /*id*/, const std::string& email)
{
    state = CS_LOBBY;
    userdata.email = email;
    if(listener)
        listener->LC_LoggedIn(userdata.email);
}

/**
 *  Lobby-Register-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyRegisterError(unsigned  /*id*/, const std::string& error) //-V524
{
    if(listener)
        listener->LC_Status_Error(error);

    ServerLost(false);
}

/**
 *  Lobby-Register-Done-Nachricht.
 */
void LobbyClient::OnNMSLobbyRegisterDone(unsigned  /*id*/)
{
    if(listener)
        listener->LC_Registered();

    Stop();
}

/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Die empfangene Serverliste
 */
void LobbyClient::OnNMSLobbyServerList(unsigned  /*id*/, const LobbyServerList& list)
{
    serverList = list;

    receivedNewServerList = true;
}

/**
 *  PlayerList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste
 */
void LobbyClient::OnNMSLobbyPlayerList(unsigned  /*id*/, const LobbyPlayerList& onlinePlayers, const LobbyPlayerList& ingamePlayers)
{
    playerList = onlinePlayers;
    for(LobbyPlayerList::const_iterator it = ingamePlayers.begin(); it != ingamePlayers.end(); ++it)
        playerList.push_back(*it);
    receivedNewPlayerList = true;
}

/**
 *  RankingList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste (Top 10)
 */
void LobbyClient::OnNMSLobbyRankingList(unsigned  /*id*/, const LobbyPlayerList& list)
{
    rankingList = list;

    receivedNewRankingList = true;
}

/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Das empfangene ServerInfo
 */
void LobbyClient::OnNMSLobbyServerInfo(unsigned  /*id*/, const LobbyServerInfo& info)
{
    serverInfo = info;

    receivedNewServerInfo = true;
}

/**
 *  Chat-Nachricht.
 *
 *  @param[in] player Der empfangene Spielername
 *  @param[in] text   Der empfangene Text
 */
void LobbyClient::OnNMSLobbyChat(unsigned  /*id*/, const std::string& player, const std::string& text)
{
    if(listener)
        listener->LC_Chat(player, text);
}

/**
 *  Lobby-Server-Add-Failed-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 */
void LobbyClient::OnNMSLobbyServerAddFailed(unsigned  /*id*/, const std::string& error)
{
    if(listener)
        listener->LC_Status_Error(error);
}

/**
 *  Lobby-Server-Add-Done-Nachricht.
 *
 *  @param[in] info Das empfangene ServerInfo
 */
void LobbyClient::OnNMSLobbyServerAdd(unsigned  /*id*/, const LobbyServerInfo& info)
{
    LOG.write(_("GameServer %s sucessfully created!\n")) % info.getName();

    state = CS_INGAME;
    isHost = true;

    // Server kann jetzt gestartet werden
    if(listener)
        listener->LC_Created();
}

/**
 *  Lobby-Ranking-Info Nachricht.
 *
 *  @param[in] email Die empfangene PlayerInfo des angefragten Spielers.
 */
void LobbyClient::OnNMSLobbyRankingInfo(unsigned  /*id*/, const LobbyPlayerInfo& player)
{
    if(listener)
        listener->LC_RankingInfo(player);
}

/**
 *  Dead-Nachricht.
 */
void LobbyClient::OnNMSDeadMsg(unsigned  /*id*/)
{
    ServerLost();
}

/**
 *  Server verloren.
 */
void LobbyClient::ServerLost(bool notifyParent)
{
    if(state != CS_STOPPED)
        LOG.write(_("Lobby client forced to stop\n"));

    Stop();

    if(listener && notifyParent)
        listener->LC_Status_ConnectionLost();
}
