// $Id: LobbyClient.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "LobbyClient.h"
#include "LobbyMessages.h"
#include <build_version.h>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p LobbyClient.
 *
 *  @author FloSoft
 */
LobbyClient::LobbyClient(void)
    : refreshserverlist(false), refreshserverinfo(false), refreshrankinglist(false), refreshplayerlist(false),
      parent(NULL), recv_queue(&LobbyMessage::create_lobby), send_queue(&LobbyMessage::create_lobby),
      state(CS_STOPPED), todo(TD_NOTHING)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p LobbyClient.
 *
 *  @author FloSoft
 */
LobbyClient::~LobbyClient(void)
{
    Stop();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Hauptschleife.
 *
 *  @author FloSoft
 */
void LobbyClient::Run(void)
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
        if(!recv_queue.recv(&socket))
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
    if(!send_queue.send(&socket, 10))
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  trennt die Verbindung mit dem LobbyServer.
 *
 *  @author FloSoft
 */
void LobbyClient::Stop()
{
    if(state != CS_CONNECT)
    {
        send_queue.push(new Message_Dead(1));
        send_queue.flush(&socket);
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  versucht einen Login in den LobbyServer.
 *
 *  @param[in] user Benutzername
 *  @param[in] pass Passwort
 *
 *  @author FloSoft
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  versucht einen Nick auf dem LobbyServer zu registrieren.
 *
 *  @param[in] user Benutzername
 *  @param[in] pass Passwort
 *
 *  @author FloSoft
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request für die Server-Liste.
 *
 *  @author FloSoft
 */
void LobbyClient::SendServerListRequest(void)
{
    send_queue.push(new LobbyMessage_ServerList(1));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request für die Player-Liste.
 *
 *  @author FloSoft
 */
void LobbyClient::SendPlayerListRequest(void)
{
    send_queue.push(new LobbyMessage_PlayerList(1));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request für die Top10-Liste.
 *
 *  @author FloSoft
 */
void LobbyClient::SendRankingListRequest(void)
{
    send_queue.push(new LobbyMessage_RankingList(1));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request für ein Serverinfo.
 *
 *  @param[in] id Server-ID
 *
 *  @author FloSoft
 */
void LobbyClient::SendServerInfoRequest(unsigned int id)
{
    if(id == 0)
        return;

    send_queue.push(new LobbyMessage_ServerInfo(id));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request für den Serverjoin.
 *
 *  @author FloSoft
 */
void LobbyClient::SendServerJoinRequest()
{
    send_queue.push(new LobbyMessage_Server_Join());
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schickt einen Request um die Punkte eines bestimmten Spielers auszulesen.
 *
 *  @author FloSoft
 */
void LobbyClient::SendRankingInfoRequest(const std::string name)
{
    send_queue.push(new LobbyMessage_Lobby_Ranking_Info(name));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt eine Chatnachricht.
 *
 *  @param[in] text Der Text
 *
 *  @author FloSoft
 */
void LobbyClient::SendChat(std::string text)
{
    send_queue.push(new LobbyMessage_Chat(text));
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  fügt einen Server zur Lobby hinzu.
 *
 *  @param[in] name Name des Servers.
 *
 *  @author FloSoft
 */
void LobbyClient::AddServer(std::string name, std::string map, bool has_password, unsigned short port)
{
    server.clear();
    server.setName(name);
    server.setVersion(GetWindowVersion());
    server.setPort(port);
    server.setMap(map);
    server.setPassword(has_password);

    send_queue.push(new LobbyMessage_Server_Add(server));

    LOG.lprintf("LobbyClient: GameServer %s wird erstellt ...\n", name.c_str());
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  entfernt den einem zugehörigen Server von der Lobby.
 *
 *  @author FloSoft
 */
void LobbyClient::DeleteServer()
{
    send_queue.push(new LobbyMessage_Server_Delete());

    server.clear();
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  aktualisiert den Kartennamen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] map Kartenname
 *
 *  @author FloSoft
 */
void LobbyClient::UpdateServer(std::string map)
{
    server.setMap(map);

    send_queue.push(new LobbyMessage_Server_Update_Map(map));
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  aktualisiert die Spielerzahlen des zugehörigen Servers in der Lobby.
 *
 *  @param[in] curplayer Aktuelle Anzahl Spieler
 *  @param[in] maxplayer Maximale Anzahl Spieler
 *
 *  @author FloSoft
 */
void LobbyClient::UpdateServerPlayerCount(unsigned int curplayer, unsigned int maxplayer)
{
    server.setCurPlayers(curplayer);
    server.setMaxPlayers(maxplayer);

    send_queue.push(new LobbyMessage_Server_Update_Player(curplayer, maxplayer));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verbindet mit dem LobbyServer.
 *
 *  @return @p true bei Erfolg, @p false bei Fehler
 *
 *  @author FloSoft
 */
bool LobbyClient::Connect(const std::string& server, const unsigned int port, const bool use_ipv6)
{
    // Verbinden
    if(!socket.Connect( server.c_str(), port, use_ipv6 ))
    {
        ServerLost();
        return false;
    }

    state = CS_CONNECT;

    if(parent)
        parent->LC_Status_Waiting();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Ping-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyPing(unsigned int id)
{
    send_queue.push(new LobbyMessage_Pong(1));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Player-ID-Nachricht.
 *
 *  @param[in] playerid Die empfangene Spieler-ID
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyID(unsigned int id, const unsigned int& playerid)
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

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Login-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyLoginError(unsigned int id, const std::string& error)
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);

    ServerLost(false);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Logged-In-Nachricht.
 *
 *  @param[in] email Die empfangene Emailadresse des Accounts
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyLoginDone(unsigned int id, const std::string& email)
{
    userdata.email = email;
    if(parent)
        parent->LC_LoggedIn(userdata.email);

    state = CS_LOBBY;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Register-Error-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyRegisterError(unsigned int id, const std::string& error)
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);

    ServerLost(false);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Register-Done-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyRegisterDone(unsigned int id)
{
    if(parent)
        parent->LC_Registered();

    Stop();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Die empfangene Serverliste
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyServerList(unsigned int id, const LobbyServerList& list)
{
    serverlist = list;

    refreshserverlist = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  PlayerList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyPlayerList(unsigned int id, const LobbyPlayerList& list)
{
    playerlist = list;

    refreshplayerlist = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  RankingList-Nachricht.
 *
 *  @param[in] error Die empfangene Spielerliste (Top 10)
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyRankingList(unsigned int id, const LobbyPlayerList& list)
{
    rankinglist = list;

    refreshrankinglist = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  ServerList-Nachricht.
 *
 *  @param[in] error Das empfangene ServerInfo
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyServerInfo(unsigned int id, const LobbyServerInfo& info)
{
    serverinfo = info;

    refreshserverinfo = true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Chat-Nachricht.
 *
 *  @param[in] player Der empfangene Spielername
 *  @param[in] text   Der empfangene Text
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyChat(unsigned int id, const std::string& player, const std::string& text)
{
    if(parent)
        parent->LC_Chat(player, text);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Add-Failed-Nachricht.
 *
 *  @param[in] error Die empfangene Fehlermeldung
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyServerAddFailed(unsigned int id, const std::string& error)
{
    this->error = error;
    if(parent)
        parent->LC_Status_Error(this->error);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Add-Done-Nachricht.
 *
 *  @param[in] info Das empfangene ServerInfo
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyServerAdd(unsigned int id, const LobbyServerInfo& info)
{
    server = info;

    LOG.lprintf("LobbyClient: GameServer %s erfolgreich erstellt!\n", server.getName().c_str());

    // Server kann jetzt gestartet werden
    if(parent)
        parent->LC_Created();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Ranking-Info Nachricht.
 *
 *  @param[in] email Die empfangene PlayerInfo des angefragten Spielers.
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSLobbyRankingInfo(unsigned int id, const LobbyPlayerInfo& player)
{
    if(parent)
        parent->LC_RankingInfo(player);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Dead-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyClient::OnNMSDeadMsg(unsigned int id)
{
    ServerLost();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Server verloren.
 *
 *  @author FloSoft
 */
void LobbyClient::ServerLost(bool message)
{
    if(state != CS_STOPPED)
        LOG.lprintf("lobby client forced to stop\n");

    Stop();

    //if(GAMECLIENT.GetState() == GameClient::CS_GAME && window && GLOBALVARS.ingame)
    //  static_cast<dskGameInterface*>(window)->messenger.AddMessage("", 0, CD_SYSTEM, _("Lost connection to lobby!"), COLOR_RED);
    if(parent && message)
        parent->LC_Status_ConnectionLost();
}
