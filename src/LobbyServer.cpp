// Copyright (c) 2005 - 2016 Settlers Freaks (sf-team at siedler25.org)
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
#include "libLobbyDefines.h" // IWYU pragma: keep
#include "LobbyServer.h"

#include "MySQL.h"
#include "LobbyProtocol.h"
#include "libutil/src/error.h"
#include <sstream>
#include <cstring>
#include <cerrno>
#ifndef _WIN32
#   include <unistd.h>
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p LobbyServer.
 *
 *  @author FloSoft
 */
LobbyServer::LobbyServer(void)
    : stop(false)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p LobbyServer.
 *
 *  @author FloSoft
 */
LobbyServer::~LobbyServer(void)
{
    MYSQLCLIENT.Disconnect();

    serverSock_.Close();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Startet den Server.
 *
 *  @param[in] port       Serverport
 *  @param[in] mysql_host MySQL-Hostname
 *  @param[in] mysql_user MySQL-Username
 *  @param[in] mysql_pass MySQL-Passwort
 *  @param[in] mysql_db   MySQL-Datenbank
 *
 *  @return @p 0 für OK, größer @p 0 für Fehler
 *
 *  @author FloSoft
 */
int LobbyServer::Start(unsigned short port, std::string mysql_host, std::string mysql_user, std::string mysql_pass, std::string mysql_db)
{
    bool use_ipv6 = false;

#ifdef USE_IPV6
    use_ipv6 = true;
#endif

    if(!serverSock_.Listen(port, use_ipv6))
        return s25Util::error(std::string("Error starting the Server: ") + strerror(errno));

    if(!MYSQLCLIENT.Connect(mysql_host, mysql_user, mysql_pass, mysql_db))
        return s25Util::error("Error connecting to the MySQL-Server\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Nachrichtenschleife.
 *
 *  @author FloSoft
 */
int LobbyServer::Run(void)
{
    // Clients testen (auf timeout usw)
    if(!CheckClientTimeouts())
        return 2;

    // neue Clients verbinden
    if(!CheckForNewClients())
        return 3;

    // Daten weiterleiten
    if(!ProcessMessages())
        return 4;

    // ggf. stoppen
    if(stop == true)
        return 100;

#ifdef _WIN32
    Sleep(20);
#else
    usleep(20);
#endif

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft aktuelle Clients.
 *
 *  @author FloSoft
 */
bool LobbyServer::CheckClientTimeouts()
{
    while(players_kill.size() > 0)
    {
        LobbyPlayerMapIterator it = players.find(players_kill.back());
        if(it != players.end())
            players.erase(it);
        players_kill.pop_back();
    }

    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        // ggf. Ping senden
        p.checkPing();

        // auf Timeout prüfen
        if(p.checkTimeout())
            Disconnect(p);
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft auf neue Clients.
 *
 *  @author FloSoft
 */
bool LobbyServer::CheckForNewClients()
{
    SocketSet set;

    set.Add(serverSock_);
    if( set.Select(0, 0) > 0)
    {
        if(set.InSet(serverSock_))
        {
            Socket client = serverSock_.Accept();
            if(!client.isValid())
                return false;

            unsigned playerid = static_cast<unsigned>(players.size());
            while(players.find(playerid) != players.end())
                ++playerid;

            LOG.write("New client connected from %s\n") % client.GetPeerIP();

            LobbyPlayer p;

            p.attach(client, playerid);

            players[playerid] = p;

            LOG.write("Player-ID %d\n") % playerid;

            p.Send(new LobbyMessage_Id(playerid), true);
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet alle Nachrichten der Clients.
 *
 *  @author FloSoft
 */
bool LobbyServer::ProcessMessages()
{
    SocketSet set;

    // In einem SocketSet alle Clients hinzufügen und gucken, ob etwas empfangen wurde
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        if(!p.isFree())
            p.addToSet(set);
    }

    if(set.Select(0, 0) > 0)
    {
        for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
        {
            LobbyPlayer& p = it->second;

            if(p.inSet(set) && !p.Receive())
                Disconnect(p);
        }
    }

    set.Clear();

    // In einem SocketSet alle Clients hinzufügen und gucken, ob fehler aufgetreten sind
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        if(!p.isFree())
            p.addToSet(set);
    }

    if(set.Select(0, 2) > 0)
    {
        LOG.write("Error on Sockets\n");

        for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
        {
            LobbyPlayer& p = it->second;

            if( p.inSet(set) )
            {
                LOG.write("Player %d: Connection lost\n") % p.getId();
                Disconnect(p);
            }
        }
    }

    // Nachrichten-Queues der Spieler abschicken (max 10 Nachrichten pro Spieler pro Runde)
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        if(!p.Send())
            Disconnect(p);
        else
            p.Run(this);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Todes-Nachricht eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSDead(unsigned id)
{
    LobbyPlayerMapIterator it = players.find(id);
    if(it != players.end())
        Disconnect(it->second);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Login-Nachricht eines Clients.
 *
 *  @param[in] message Nachricht, welche ausgeführt wird
 *  @param[in] client  die Nummer des Clients
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyLogin(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& version)
{
    LobbyPlayer& player = players[id];

    // Protokollversion prüfen
    if(!CheckProtocolVersion(revision, user, player))
        return;

    std::string email;
    if(MYSQLCLIENT.LoginUser(user, pass, email, player.getPeerIP()))
    {
        LobbyPlayer* oldPlayer = GetPlayer(user);
        if(oldPlayer && oldPlayer->isLoggedIn())
        {
            LOG.write("User %s@%s already logged on (slot %d == %d)!\n") % user % player.getPeerIP() % id % oldPlayer->getId();
            player.Send(new LobbyMessage_Login_Error("Already logged in. On connection loss just wait a bit then try again."));
            // alten Spieler rauswerfen
            Disconnect(*oldPlayer);
        }else
        {
            LOG.write("User %s@%s logged on\n") % user % player.getPeerIP();

            player.occupy(user, email, version);
            player.Send(new LobbyMessage_Login_Done(email));

            SendToAll(LobbyMessage_Chat("SYSTEM", user + " hat die Lobby betreten"));

            SendServerList(0xFFFFFFFF);
            SendPlayerList(0xFFFFFFFF);
        }
    }
    else
    {
        LOG.write("User %s invalid (password %s wrong?)\n") % user % "********";
        player.Send(new LobbyMessage_Login_Error("User/password combination is unknown!"));
        Disconnect(player);
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Register-Nachricht eines Clients.
 *
 *  @param[in] message Nachricht, welche ausgeführt wird
 *  @param[in] client  die Nummer des Clients
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyRegister(unsigned id, const unsigned revision, const std::string& user, const std::string& pass, const std::string& email)
{
    LobbyPlayer& player = players[id];

    // Protokollversion prüfen
    if(!CheckProtocolVersion(revision, user, player))
        return;

    /*if(MYSQLCLIENT.RegisterUser(user, pass, email))
    {
        LOG.write(("User %s registered\n", user);

        player.Send(new LobbyMessage_Register_Done(1));
    }
    else
    {
        LOG.write(("User %s failed to register\n", user);

        player.Send(new LobbyMessage_Register_Error("Registrierung fehlgeschlagen: Datenbankfehler oder Benutzer existiert schon"));

        Disconnect(player);
    }*/
    LOG.write("User %s tried to register\n") % user;

    player.Send(new LobbyMessage_Register_Error("To register, you have to create a valid board account at\nhttp://forum.siedler25.org\nat the moment.\n"));

    Disconnect(player);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Pong-Nachricht eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyPong(unsigned id)
{
    LobbyPlayer& player = players[id];

    player.gotPing();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Chat-Nachricht eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyChat(unsigned id, const std::string& to, const std::string& text)
{
    if(text.empty())
        return;

    LobbyPlayer& player = players[id];

    if(player.getName() == "LobbyBot")
    {
        if(text == "!kick" || !text.compare(0, 6, "!kick "))
        {
            LobbyPlayer* p = GetPlayer(to);
            if(p)
            {
                if(text.length() > 6)
                    p->Send(new LobbyMessage_Chat(player.getName(), text.substr(6)));

                Disconnect(*p);
            }
        }
        else if(text == "!ban")
        {
            MYSQLCLIENT.SetBan(to, true);
            player.Send(new LobbyMessage_Chat(player.getName(), std::string("!ban") + to));
        }
        else if(text == "!unban")
        {
            MYSQLCLIENT.SetBan(to, false);
            player.Send(new LobbyMessage_Chat(player.getName(), std::string("!unban") + to));
        }
        else if(text == "!getinfo")
        {
            LobbyPlayer* p = GetPlayer(to);
            if(p)
            {
                std::stringstream out;
                out << "!getinfo " << p->getPeerIP() << " " << p->getEmail() << " " << p->getName();
                player.Send(new LobbyMessage_Chat(player.getName(), out.str()));
            }
        }

        // Hide all commands
        if (text[0] == '!')
            return;
    }

    // send to lobbybot only, throw away otherwise
    if (text[0] == '!')
    {
        LobbyPlayer* p = GetPlayer("LobbyBot");
        if(p)
            p->Send(new LobbyMessage_Chat(player.getName(), text));
        return;
    }

    if(!to.empty())
    {
        LobbyPlayer* p = GetPlayer(to);
        if(p)
            p->Send(new LobbyMessage_Chat(player.getName(), text));
    } else
    {
        // no player selected
        SendToAll(LobbyMessage_Chat(player.getName(), text));
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die ServerList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerList(unsigned id)
{
    SendServerList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die PlayerList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyPlayerList(unsigned id)
{
    SendPlayerList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die RankingList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyRankingList(unsigned id)
{
    SendRankingList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die ServerInfo-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerInfo(unsigned id, const unsigned& server)
{
    LobbyPlayer& player = players[id];

    LobbyServerInfo info;
    if(MYSQLCLIENT.GetServerInfo(server, &info))
        player.Send(new LobbyMessage_ServerInfo(info));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt einen Server zur Liste hinzu.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerAdd(unsigned id, const LobbyServerInfo& info)
{
    LobbyPlayer& player = players[id];

    if(player.Host(info))
        SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  aktualisiert die Spielerzahl in einem Servereintrag.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerUpdatePlayer(unsigned id, const unsigned curplayer, const unsigned maxplayer)
{
    LobbyPlayer& player = players[id];

    player.updateHost(curplayer, maxplayer);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  aktualisiert den Mapnamen in einem Servereintrag.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerUpdateMap(unsigned id, const std::string& map)
{
    LobbyPlayer& player = players[id];

    if(player.updateHost(map))
    {
        LobbyServerInfo info;
        MYSQLCLIENT.GetServerInfo(player.getServerId(), &info);

        std::stringstream text;
        text << player.getName() << " created the server " << info.getName();
        SendToAll(LobbyMessage_Chat("SYSTEM", text.str()));

        // Spielerliste aktualisieren
        SendServerList(0xFFFFFFFF);
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  entfernt einen Server aus der Liste.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerDelete(unsigned id)
{
    LobbyPlayer& player = players[id];

    player.NoHost();

    SendToAll(LobbyMessage_Chat("SYSTEM", player.getName() + " is available"));

    // Spielerliste aktualisieren
    SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  sagt der Lobby das ein Spieler einem Spiel beigetreten ist.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerJoin(unsigned id)
{
    LobbyPlayer& player = players[id];

    player.makeClient();

	std::stringstream text;
	text << "Player \"" << player.getName() << "\" joined a server";

    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;
		if(p.getName() == "LobbyBot")
		{
			p.Send(new LobbyMessage_Chat("SYSTEM", text.str()));
			break;
		}
	}

    // Spielerliste aktualisieren
    SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
*  schickt eine Nachricht an alle Clients.
*
*  @param[in] m Die Nachricht
*
*  @author FloSoft
*/
void LobbyServer::SendToAll(const LobbyMessage& msg)
{
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        // ist der Slot Belegt, dann Nachricht senden
        if(p.isOccupied())
            p.Send(dynamic_cast<LobbyMessage*>(msg.duplicate()));
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  trennt die Verbindung zu einem Client.
 *
 *  @param[in] p der Spieler.
 *
 *  @author FloSoft
 */
void LobbyServer::Disconnect(LobbyPlayer& p)
{
    LobbyPlayerMapIterator it = players.find(p.getId());
    players_kill.push_back(it->first);

    if(!p.getName().empty())
        SendToAll(LobbyMessage_Chat("SYSTEM", p.getName() + " left the lobby"));

    // schließen
    p.NoHost();
    p.detach();

    SendServerList(0xFFFFFFFF);
    SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt die Serverliste an einen Client.
 *
 *  @param[in] id die Nummer des Clients (0xFFFFFFFF für Broadcast)
 *
 *  @author FloSoft
 */
void LobbyServer::SendServerList(unsigned id)
{
    LobbyServerList list;

    if(!MYSQLCLIENT.GetServerList(&list))
        LOG.write("Failed to lookup Serverlist!\n");

    if(id == 0xFFFFFFFF)
        SendToAll(LobbyMessage_ServerList(list));
    else
        players[id].Send(new LobbyMessage_ServerList(list));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt die Playerliste an einen Client.
 *
 *  @param[in] id die Nummer des Clients (0xFFFFFFFF für Broadcast)
 *
 *  @author FloSoft
 */
void LobbyServer::SendPlayerList(unsigned id)
{
    LobbyPlayerList list;

    unsigned count = 0;
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& p = it->second;

        if(p.getName() == "LobbyBot")
            continue;

        if(p.isOccupied() && !p.isHost() && !p.isClient() )
        {
            if(p.getPunkte() == 0 && !MYSQLCLIENT.GetRankingInfo(p))
                LOG.write("Failed to lookup Ranking of player %s!\n") % p.getName();

            ++count;
            list.push_back(p);
        }
    }

    if(id == 0xFFFFFFFF)
        SendToAll(LobbyMessage_PlayerList(list));
    else
        players[id].Send(new LobbyMessage_PlayerList(list));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt die Rankingliste an einen Client.
 *
 *  @param[in] client die Nummer des Clients (0xFFFFFFFF für Broadcast)
 *
 *  @author FloSoft
 */
void LobbyServer::SendRankingList(unsigned id)
{
    LobbyPlayerList list;

    if(!MYSQLCLIENT.GetRankingList(&list))
        LOG.write("Failed to lookup Ranking!\n");

    if(id == 0xFFFFFFFF)
        SendToAll(LobbyMessage_RankingList(list));
    else
        players[id].Send(new LobbyMessage_RankingList(list));
}

LobbyPlayer* LobbyServer::GetPlayer(const std::string& name)
{
    for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
    {
        LobbyPlayer& player = it->second;
        if(player.isOccupied() && player.getName() == name)
            return &player;
    }
    return NULL;
}

bool LobbyServer::CheckProtocolVersion(unsigned userVersion, const std::string& userName, LobbyPlayer& player)
{
    if(userVersion != LOBBYPROTOCOL_VERSION)
    {
        LOG.write("User %s@%s invalid (protocoll version wrong)\n") % userName % player.getPeerIP();

        // do we've got a revision? or is it so damn old that it does not send a revision?
        if((userVersion & 0xFF0000FF) == 0xFF0000FF)
        {
            // newer client
            player.Send(new LobbyMessage_Login_Error("Wrong protocal version! Program is to old."));
        } else
        {
            // really old client (<= 0.6)
            player.Send(new LobbyMessage_Login_Error06("Wrong protocal version! Program is to old."));
        }

        Disconnect(player);
        return false;
    } else
        return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Ranking-Info Nachricht.
 *
 *  @param[in] email Die PlayerInfo des angefragten Spielers.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyRankingInfo(unsigned id, const LobbyPlayerInfo& player)
{
    LobbyPlayerInfo p = player;

    if(!MYSQLCLIENT.GetRankingInfo(p))
        LOG.write("Failed to lookup Ranking of player %s!\n") % p.getName();

    SendToAll(LobbyMessage_Lobby_Ranking_Info(p));
}
