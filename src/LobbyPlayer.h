// $Id: LobbyPlayer.h 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#ifndef LOBBYPLAYER_H_INCLUDED
#define LOBBYPLAYER_H_INCLUDED

#pragma once

#include "MessageQueue.h"
#include "LobbyPlayerInfo.h"
#include "LobbyMessages.h"

class LobbyPlayer : public LobbyPlayerInfo
{
    public:
        /// Konstruktor von @p LobbyPlayer.
        LobbyPlayer(void);
        /// Destruktor von @p LobbyPlayer.
        ~LobbyPlayer(void);

    public:
        /// räumt den Spieler auf.
        void clear(void);

        void sendPing(void);
        bool checkPing(void);
        bool checkTimeout(void);
        void gotPing(void);

        void attach(const Socket& socket, const unsigned int& playerid);
        void detach(void);
        void occupy(const std::string& user, const std::string& email, const std::string& version);
        std::string getPeerIP() { return socket.GetPeerIP(); }

        void addToSet(SocketSet& set) { set.Add(socket); }
        bool inSet(SocketSet& set) { return set.InSet(socket); }

        bool isFree(void)     { return (playerstate == PS_FREE);     }
        bool isOccupied(void) { return (playerstate == PS_OCCUPIED); }
        bool isReserved(void) { return (playerstate == PS_RESERVED); }
        bool isLoggedIn(void) { return (loggedin && playerstate == PS_OCCUPIED); }
        bool isHost(void)     { return host; }
        bool isClient(void)   { return client; }

        bool Receive(void);
        bool Send(LobbyMessage* m = NULL, bool flush = false);
        void Run(LobbyMessageInterface* callback);

        unsigned int getServerId(void) { return serverid; }

        bool Host(LobbyServerInfo info);
        void NoHost(void);
        bool updateHost(const unsigned int curplayer, const unsigned int maxplayer);
        bool updateHost(const std::string& map);

        void Client(void) { client = true; }

    private:
        MessageQueue send_queue;
        MessageQueue recv_queue;

    private:
        enum PS
        {
            PS_FREE = 0,
            PS_RESERVED,
            PS_OCCUPIED
        } playerstate;

        Socket socket;

        bool pinging;
        bool loggedin;
        unser_time_t lastping;
        unser_time_t connectiontime;
        unsigned short ping;

        unsigned int serverid;

        bool client;
        bool host;
};

#endif // !LOBBYPLAYER_H_INCLUDED
