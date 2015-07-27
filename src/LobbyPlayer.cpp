// $Id: LobbyPlayer.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include <build_version.h>
#include "LobbyPlayer.h"

#include "MySQL.h"
#include "LobbyMessage.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p LobbyPlayer.
 *
 *  @author FloSoft
 */
LobbyPlayer::LobbyPlayer(void) : send_queue(&LobbyMessage::create_lobby), recv_queue(&LobbyMessage::create_lobby)
{
    clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p LobbyPlayer.
 *
 *  @author FloSoft
 */
LobbyPlayer::~LobbyPlayer(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  räumt den Spieler auf.
 *
 *  @author FloSoft
 */
void LobbyPlayer::clear(void)
{
    playerstate = PS_FREE;

    send_queue.clear();
    recv_queue.clear();

    pinging = false;
    loggedin = false;

    lastping = 0;
    connectiontime = 0;
    ping = 0;

    client = false;
    host = false;
    serverid = 0;

    LobbyPlayerInfo::clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  sendet einen Ping.
 *
 *  @author FloSoft
 */
void LobbyPlayer::sendPing(void)
{
    lastping = TIME.CurrentTick();

    Send(new LobbyMessage_Ping(1));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob ein Ping gesendet werden muss.
 *
 *  @author FloSoft
 */
bool LobbyPlayer::checkPing(void)
{
    // Ping erzeugen
    if( (playerstate == PS_OCCUPIED) && (pinging == false) && ( (TIME.CurrentTick() - lastping) > 1000) )
    {
        sendPing();
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  prüft ob ein Ping gesendet werden muss.
 *
 *  @author FloSoft
 */
bool LobbyPlayer::checkTimeout(void)
{
    // Bei sehr hohen Pings und längeren Reservierungen trennen wir uns von
    // den Spielern ( ping timeout )
    if( (playerstate == PS_OCCUPIED) && (pinging == false) && ( (TIME.CurrentTick() - lastping) > 5000) )
    {
        LOG.lprintf("Kicking authenticated player %s (Ping timeout)\n", getName().c_str());
        return true;
    }

    if( (playerstate == PS_RESERVED) && ( (TIME.CurrentTick() - connectiontime) > 10000) )
    {
        LOG.lprintf("Removing unauthenticated client (Ping timeout)\n");
        return true;
    }

    return false;
}

void LobbyPlayer::attach(const Socket& socket, const unsigned int& playerid)
{
    detach();

    this->socket = socket;
    this->playerid = playerid;

    playerstate = PS_RESERVED;

    connectiontime = TIME.CurrentTick();
}

void LobbyPlayer::detach(void)
{
    NoHost();

    if(!isFree())
        LOG.lprintf("Removing player %s\n", getName().c_str());

    Send(NULL, true);
    socket.Close();

    clear();
}

bool LobbyPlayer::Receive(void)
{
    if(!recv_queue.recv( &socket ))
    {
        LOG.lprintf("SERVER: Receiving message for player %d failed\n", playerid);
        return false;
    }
    return true;
}

bool LobbyPlayer::Send(LobbyMessage* m, bool flush)
{
    if(m)
    {
        // Nachricht einhängen.
        send_queue.push(m);

        // und nur wenn wir nicht flushen beenden.
        if(!flush)
            return true;
    }

    // Alle Nachrichten abschicken.
    if(flush)
    {
        send_queue.flush(&socket);
        return true;
    }

    if( !send_queue.send( &socket, 10, 512 ) )
    {
        LOG.lprintf("SERVER: Sending message for player %d failed\n", playerid);
        return false;
    }
    return true;
}

void LobbyPlayer::Run(LobbyMessageInterface* callback)
{
    // recv-queue abarbeiten
    while(recv_queue.count() > 0)
    {
        recv_queue.front()->run(callback, playerid);
        recv_queue.pop();
    }
}

void LobbyPlayer::occupy(const std::string& user, const std::string& email, const std::string& version)
{
    loggedin = true;
    playerstate = PS_OCCUPIED;
    lastping = TIME.CurrentTick();

    setName(user);
    setEmail(email);
    setVersion(version);

    std::stringstream text;

    text.str("");
    text << GetWindowTitle();
    Send(new LobbyMessage_Chat("Hinweis", text.str()));

    text.str("");
    text << "Serverversion: " << GetWindowVersion() << "-r" << GetWindowRevision();
    Send(new LobbyMessage_Chat("Hinweis", text.str()));
}

void LobbyPlayer::gotPing(void)
{
    pinging = false;

    ping = (unsigned short)(TIME.CurrentTick() - lastping);

    lastping = TIME.CurrentTick();

    if(host && serverid != 0)
        MYSQLCLIENT.UpdateServerPing(serverid, ping);
}

bool LobbyPlayer::Host(LobbyServerInfo info)
{
    info.setHost(socket.GetPeerIP());

    if(MYSQLCLIENT.AddServer(&info))
    {
        LOG.lprintf("Player %s creates game '%s'\n", getName().c_str(), info.getName().c_str());

        Send(new LobbyMessage_Server_Add(info));

        host = true;
        serverid = info.getId();

        return true;
    }

    LOG.lprintf("Can't create game\n");

    Send(new LobbyMessage_Server_Add_Failed("Datenbankfehler beim Erstellen.\nName ungültig oder schon belegt."));

    return false;
}

void LobbyPlayer::NoHost(void)
{
    if(host == true && serverid != 0)
        MYSQLCLIENT.DeleteServer(serverid);

    client = false;
    host = false;
    serverid = 0;
}

bool LobbyPlayer::updateHost(const unsigned int curplayer, const unsigned int maxplayer)
{
    if(host = true && serverid != 0)
        return MYSQLCLIENT.UpdateServerPC(serverid, curplayer, maxplayer);
    return false;
}

bool LobbyPlayer::updateHost(const std::string& map)
{
    if(host = true && serverid != 0)
        return MYSQLCLIENT.UpdateServer(serverid, map);
    return false;
}
