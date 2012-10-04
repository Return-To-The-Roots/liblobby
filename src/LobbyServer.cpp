// $Id: LobbyServer.cpp 8387 2012-10-04 16:19:07Z FloSoft $
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
#include "LobbyServer.h"

#include "MySQL.h"
#include "LobbyProtocol.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

	server.Close();
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

	if(!server.Listen(port, use_ipv6))
		return error("Fehler beim Starten des Server: %s\n", strerror(errno));

	if(!MYSQLCLIENT.Connect(mysql_host, mysql_user, mysql_pass, mysql_db))
		return error("Fehler beim Verbinden mit dem MySQL-Server\n");

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
	if(!Test())
		return 2;

	// neue Clients verbinden
	if(!Await())
		return 3;

	// Daten weiterleiten
	if(!Forward())
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
bool LobbyServer::Test()
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
		LobbyPlayer &p = it->second;

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
bool LobbyServer::Await()
{
	SocketSet set;
	int playerid = 0xFFFFFFFF;

	set.Add(server);
	if( set.Select(0, 0) > 0)
	{
		if(set.InSet(server))
		{
			Socket tmp;
			if(!server.Accept(tmp))
				return false;

			playerid = players.size();
			while(players.find(playerid) != players.end())
				++playerid;

			LOG.lprintf("New client connected from %s\n", tmp.GetPeerIP().c_str());

			LobbyPlayer p;

			p.attach(tmp, playerid);

			players[playerid] = p;

			LOG.lprintf("Player-ID %d\n", playerid);

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
bool LobbyServer::Forward()
{
	SocketSet set;

	bool not_empty = false;
	unsigned int max_not_empty = 0;

	// erstmal auf Daten überprüfen
	/*do
	{*/
		// In einem SocketSet alle Clients hinzufügen und gucken, ob etwas empfangen wurde
		for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
		{
			LobbyPlayer &p = it->second;

			if(p.isReserved() || p.isOccupied())
				p.addToSet(set);
		}

		//not_empty = false;

		if(set.Select(0, 0) > 0)
		{
			for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
			{
				LobbyPlayer &p = it->second;

				if( p.inSet(set) )
				{
					// nachricht empfangen
					if(!p.Receive())
						Disconnect(p);
					/*else if(p.isOccupied())
					{
						not_empty = true;
						++max_not_empty;
					}*/
				}
			}
		}

		set.Clear();
	//} while(not_empty && max_not_empty < 10000);

	// In einem SocketSet alle Clients hinzufügen und gucken, ob fehler aufgetreten sind
	for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
	{
		LobbyPlayer &p = it->second;

		if(p.isReserved() || p.isOccupied())
			p.addToSet(set);
	}

	if(set.Select(0, 2) > 0)
	{
		LOG.lprintf("Error on Sockets\n");

		for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
		{
			LobbyPlayer &p = it->second;

			if( p.inSet(set) )
			{
				LOG.lprintf("Player %d: Connection lost\n", p.getId() );
				Disconnect(p);
			}
		}
	}

	// Nachrichten-Queues der Spieler abschicken (max 10 Nachrichten pro Spieler pro Runde)
	for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
	{
		LobbyPlayer &p = it->second;

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
void LobbyServer::OnNMSDead(unsigned int id)
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
void LobbyServer::OnNMSLobbyLogin(unsigned int id, const unsigned int revision, const std::string &user, const std::string &pass, const std::string &version)
{
	LobbyPlayer &player = players[id];

	std::string email;

	// Protokollversion prüfen
	if(revision != LOBBYPROTOCOL_VERSION)
	{
		// zu alt
		LOG.lprintf("User %s@%s invalid (protocoll version wrong)\n", user.c_str(), player.getPeerIP().c_str());

		// do we've got a revision? or is it so damn old that it does not send a revision?
		if( (revision & 0xFF0000FF) == 0xFF0000FF)
		{
			// newer client
			player.Send(new LobbyMessage_Login_Error("Falsche Protokollversion! Programmversion ist zu alt."));
		}
		else
		{
			// really old client (<= 0.6)
			player.Send(new LobbyMessage_Login_Error06("Falsche Protokollversion! Programmversion ist zu alt."));
		}

		Disconnect(player);
	}
	else
	{
		// prüfen
		if(MYSQLCLIENT.LoginUser(user, pass, email))
		{
			bool found = false;

			for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
			{
				LobbyPlayer &p = it->second;
				
				if(p.isLoggedIn() && p.getName() == user)
				{
					LOG.lprintf("User %s@%s already logged on (slot %d == %d)!\n", user.c_str(), player.getPeerIP().c_str(), id, p.getId());

					player.Send(new LobbyMessage_Login_Error("Bereits eingeloggt! Bei Verbindungsabbruch einen Moment warten."));

					// alten Spieler rauswerfen
					Disconnect(p);

					found = true;
				}
			}

			if(!found)
			{
				LOG.lprintf("User %s@%s logged on\n", user.c_str(), player.getPeerIP().c_str());

				player.Send(new LobbyMessage_Login_Done(email));

				player.occupy(user, version);

				std::stringstream text;
				text << user << " hat die Lobby betreten";

				LobbyMessage *m = new LobbyMessage_Chat("SYSTEM", text.str());
				SendToAll(m);
				delete m;

				SendServerList(0xFFFFFFFF);
				SendPlayerList(0xFFFFFFFF);
			}
		}
		else
		{
			LOG.lprintf("User %s invalid (password %s wrong?)\n", user.c_str(), "********");

			player.Send(new LobbyMessage_Login_Error("Benutzer/Passwort-Kombination unbekannt!"));

			Disconnect(player);
		}
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
void LobbyServer::OnNMSLobbyRegister(unsigned int id, const unsigned int revision, const std::string &user, const std::string &pass, const std::string &email)
{
	LobbyPlayer &player = players[id];

	// Protokollversion prüfen
	if(revision != LOBBYPROTOCOL_VERSION)
	{
		// zu alt
		LOG.lprintf("User %s invalid (protocoll version wrong)\n", user.c_str());

		// do we've got a revision? or is it so damn old that it does not send a revision?
		if( (revision & 0xFF0000FF) == 0xFF0000FF)
		{
			// newer client
			player.Send(new LobbyMessage_Register_Error("Falsche Protokollversion! Programmversion ist zu alt."));
		}
		else
		{
			// really old client (<= 0.6)
			player.Send(new LobbyMessage_Register_Error06("Falsche Protokollversion! Programmversion ist zu alt."));
		}

		Disconnect(player);
	}
	else
	{
		/*if(MYSQLCLIENT.RegisterUser(user, pass, email))
		{
			LOG.lprintf("User %s registered\n", user.c_str());

			player.Send(new LobbyMessage_Register_Done(1));
		}
		else
		{
			LOG.lprintf("User %s failed to register\n", user.c_str());

			player.Send(new LobbyMessage_Register_Error("Registrierung fehlgeschlagen: Datenbankfehler oder Benutzer existiert schon"));

			Disconnect(player);
		}*/
		LOG.lprintf("User %s tried to register\n", user.c_str());

		player.Send(new LobbyMessage_Register_Error("To register, you have to create a valid board account at\nhttp://forum.siedler25.org\nat the moment.\n"));

		Disconnect(player);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Pong-Nachricht eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyPong(unsigned int id)
{
	LobbyPlayer &player = players[id];

	player.gotPing();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die Chat-Nachricht eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyChat(unsigned int id, const std::string &to, const std::string &text)
{
	if(text.size() <= 0)
		return;

	LobbyPlayer &player = players[id];

	if (player.getName() == "LobbyBot")
	{
		if (!text.compare(0, 6, "!kick ") )
		{
			for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
			{
				LobbyPlayer &p = it->second;

				if(p.isOccupied() && p.getName() == to)
				{
					if (text.size() > 6)
						p.Send(new LobbyMessage_Chat(player.getName(), text.substr(6)));
					Disconnect(p);
					break;
				}
			}

			return;
		} else if (!text.compare("!ban"))
		{
			int id = atoi(to.c_str());

			if (id != 0)
			{
				MYSQLCLIENT.SetBan(id, 1);
				player.Send(new LobbyMessage_Chat(player.getName(), "Banned."));
			}

			return;
		} else if (!text.compare("!unban"))
		{
			int id = atoi(to.c_str());

			if (id != 0)
			{
				MYSQLCLIENT.SetBan(id, 0);
				player.Send(new LobbyMessage_Chat(player.getName(), "Unbanned."));
			}

			return;
		} else if (!text.compare("!getip"))
		{
			for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
			{
				LobbyPlayer &p = it->second;

				if(p.isOccupied() && (p.getName() == to))
				{
					player.Send(new LobbyMessage_Chat(player.getName(), player.getPeerIP()));
					break;
				}
			}

			return;
		}
	}

	if(to.size() > 0)
	{
		for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
		{
			LobbyPlayer &p = it->second;
			if(p.isOccupied() && p.getName() == to)
			{
				p.Send(new LobbyMessage_Chat(player.getName(), text));
				return;
			}
		}
	}

	// did not found player or none selected
	LobbyMessage *m = new LobbyMessage_Chat(player.getName(), text);
	SendToAll(m);
	delete m;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die ServerList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerList(unsigned int id)
{
	SendServerList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die PlayerList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyPlayerList(unsigned int id)
{
	SendPlayerList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die RankingList-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyRankingList(unsigned int id)
{
	SendRankingList(id);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verarbeitet die ServerInfo-Anforderung eines Clients.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerInfo(unsigned int id, const unsigned int &server)
{
	LobbyPlayer &player = players[id];

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
void LobbyServer::OnNMSLobbyServerAdd(unsigned int id, const LobbyServerInfo &info)
{
	LobbyPlayer &player = players[id];

	if(player.Host(info))
		SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  aktualisiert die Spielerzahl in einem Servereintrag.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerUpdatePlayer(unsigned int id, const unsigned int curplayer, const unsigned int maxplayer)
{
	LobbyPlayer &player = players[id];

	player.updateHost(curplayer, maxplayer);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  aktualisiert den Mapnamen in einem Servereintrag.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerUpdateMap(unsigned int id, const std::string &map)
{
	LobbyPlayer &player = players[id];

	if(player.updateHost(map))
	{
		LobbyServerInfo info;
		MYSQLCLIENT.GetServerInfo(player.getServerId(), &info);

		std::stringstream text;
		text << player.getName() << " hat den Server " << info.getName() << " erstellt";
		LobbyMessage *m = new LobbyMessage_Chat("SYSTEM", text.str());
		SendToAll(m);
		delete m;

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
void LobbyServer::OnNMSLobbyServerDelete(unsigned int id)
{
	LobbyPlayer &player = players[id];

	player.NoHost();

	std::stringstream text;
	text << player.getName() << " ist wieder anwesend";
	LobbyMessage *m = new LobbyMessage_Chat("SYSTEM", text.str());
	SendToAll(m);
	delete m;

	// Spielerliste aktualisieren
	SendPlayerList(0xFFFFFFFF);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  sagt der Lobby das ein Spieler einem Spiel beigetreten ist.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyServerJoin(unsigned int id)
{
	LobbyPlayer &player = players[id];

	player.Client();

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
void LobbyServer::SendToAll(const LobbyMessage *msg)
{
	for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
	{
		LobbyPlayer &p = it->second;

		// ist der Slot Belegt, dann Nachricht senden
		if(p.isOccupied())
			p.Send(dynamic_cast<LobbyMessage*>(msg->duplicate()));
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
void LobbyServer::Disconnect(LobbyPlayer &p)
{
	LobbyPlayerMapIterator it = players.find(p.getId());
	players_kill.push_back(it->first);

	if(!p.getName().empty())
	{
		std::stringstream text;
		text << p.getName() << " hat die Lobby verlassen";
		LobbyMessage *m = new LobbyMessage_Chat("SYSTEM", text.str());
	
		SendToAll(m);
		delete m;
	}

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
void LobbyServer::SendServerList(unsigned int id)
{
	LobbyServerList list;

	if(!MYSQLCLIENT.GetServerList(&list))
		LOG.lprintf("Failed to lookup Serverlist!\n");

	LobbyMessage *m = new LobbyMessage_ServerList(list);
	if(id == 0xFFFFFFFF)
	{
		SendToAll(m);
		delete m;
	}
	else
		players[id].Send(m);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt die Playerliste an einen Client.
 *
 *  @param[in] id die Nummer des Clients (0xFFFFFFFF für Broadcast)
 *
 *  @author FloSoft
 */
void LobbyServer::SendPlayerList(unsigned int id)
{
	LobbyPlayerList list;

	unsigned int count = 0;
	for(LobbyPlayerMapIterator it = players.begin(); it != players.end(); ++it)
	{
		LobbyPlayer &p = it->second;

		if(p.getName() == "LobbyBot")
			continue;

		if(p.isOccupied() && !p.isHost() && !p.isClient() )
		{
			if(p.getPunkte() == 0 && !MYSQLCLIENT.GetRankingInfo(p))
                		LOG.lprintf("Failed to lookup Ranking of player %s!\n", p.getName().c_str());

			++count;
			list.push_back(p);
		}
	}

	LobbyMessage *m = new LobbyMessage_PlayerList(list);
	if(id == 0xFFFFFFFF)
	{
		SendToAll(m);
		delete m;
	}
	else
		players[id].Send(m);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  verschickt die Rankingliste an einen Client.
 *
 *  @param[in] client die Nummer des Clients (0xFFFFFFFF für Broadcast)
 *
 *  @author FloSoft
 */
void LobbyServer::SendRankingList(unsigned int id)
{
	LobbyPlayerList list;

	if(!MYSQLCLIENT.GetRankingList(&list))
		LOG.lprintf("Failed to lookup Ranking!\n");

	LobbyMessage *m = new LobbyMessage_RankingList(list);
	if(id == 0xFFFFFFFF)
	{
		SendToAll(m);
		delete m;
	}
	else
		players[id].Send(m);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Ranking-Info Nachricht.
 *
 *  @param[in] email Die PlayerInfo des angefragten Spielers.
 *
 *  @author FloSoft
 */
void LobbyServer::OnNMSLobbyRankingInfo(unsigned int id, const LobbyPlayerInfo &player)
{
	LobbyPlayerInfo p = player;

	if(!MYSQLCLIENT.GetRankingInfo(p))
		LOG.lprintf("Failed to lookup Ranking of player %s!\n", p.getName().c_str());

	LobbyMessage *m = new LobbyMessage_Lobby_Ranking_Info(p);
	SendToAll(m);
	delete m;
}
