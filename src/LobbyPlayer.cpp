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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h" // IWYU pragma: keep
#include <build_version.h>
#include "LobbyPlayer.h"

#include "MySQL.h"
#include "LobbyMessage.h"
#include <sstream>

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

    client_ = false;
    host_ = false;
    serverid_ = 0;

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
        LOG.write("Kicking authenticated player %s (Ping timeout)\n") % getName();
        return true;
    }

    if( (playerstate == PS_RESERVED) && ( (TIME.CurrentTick() - connectiontime) > 10000) )
    {
        LOG.write("Removing unauthenticated client (Ping timeout)\n");
        return true;
    }

    return false;
}

void LobbyPlayer::attach(const Socket& socket, const unsigned int& playerid)
{
    detach();

    this->socket = socket;
    setId(playerid);

    playerstate = PS_RESERVED;

    connectiontime = TIME.CurrentTick();
}

void LobbyPlayer::detach(void)
{
    NoHost();

    if(!isFree())
        LOG.write("Removing player %s\n") % getName();

    Send(NULL, true);
    socket.Close();

    clear();
}

bool LobbyPlayer::Receive(void)
{
    if(!recv_queue.recv( socket ))
    {
        LOG.write("SERVER: Receiving message for player %d failed\n") % getId();
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
        send_queue.flush(socket);
        return true;
    }

    if( !send_queue.send( socket, 10, 512 ) )
    {
        LOG.write("SERVER: Sending message for player %d failed\n") % getId();
        return false;
    }
    return true;
}

void LobbyPlayer::Run(LobbyMessageInterface* callback)
{
    // recv-queue abarbeiten
    while(recv_queue.count() > 0)
    {
        recv_queue.front()->run(callback, getId());
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

    Send(new LobbyMessage_Chat("Hinweis", GetWindowTitle()));

    std::stringstream text;
    text.str("");
    text << "Serverversion: " << GetWindowVersion() << "-r" << GetWindowRevision();
    Send(new LobbyMessage_Chat("Hinweis", text.str()));
}

void LobbyPlayer::gotPing(void)
{
    pinging = false;

    ping = (unsigned short)(TIME.CurrentTick() - lastping);

    lastping = TIME.CurrentTick();

    if(isHost())
        MYSQLCLIENT.UpdateServerPing(getServerId(), ping);
}

bool LobbyPlayer::Host(LobbyServerInfo info)
{
    info.setHost(socket.GetPeerIP());

    if(MYSQLCLIENT.AddServer(&info))
    {
        LOG.write("Player %s creates game '%s'\n") % getName() % info.getName();
        makeHost(info.getId());
        Send(new LobbyMessage_Server_Add(info));
        return true;
    } else{
        LOG.write("Can't create game\n");
        Send(new LobbyMessage_Server_Add_Failed("Database error on create.\nName invalid or already taken."));
        return false;
    }
}

void LobbyPlayer::NoHost(void)
{
    if(isHost())
        MYSQLCLIENT.DeleteServer(getServerId());

    client_ = false;
    host_ = false;
    serverid_ = 0;
}

bool LobbyPlayer::updateHost(const unsigned int curplayer, const unsigned int maxplayer)
{
    if(isHost())
        return MYSQLCLIENT.UpdateServerPC(getServerId(), curplayer, maxplayer);
    return false;
}

bool LobbyPlayer::updateHost(const std::string& map)
{
    if(isHost())
        return MYSQLCLIENT.UpdateServer(getServerId(), map);
    return false;
}

void LobbyPlayer::makeClient()
{
    client_ = true;    
}

void LobbyPlayer::makeHost(unsigned serverId)
{
    assert(serverId);
    client_ = false;
    host_ = true;
    serverid_ = serverId;
}
