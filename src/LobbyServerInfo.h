// $Id: LobbyServerInfo.h 9359 2014-04-25 15:37:22Z FloSoft $
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
#ifndef LOBBYSERVERINFO_H_INCLUDED
#define LOBBYSERVERINFO_H_INCLUDED

#pragma once

class Serializer;

class LobbyServerInfo
{
    public:
        /// Konstruktor von @p LobbyServerInfo.
        LobbyServerInfo(void);
        /// Deserialisierungs-Konstruktor von @p LobbyServerInfo.
        LobbyServerInfo(const unsigned playerid, Serializer* ser);
        /// leert die Informationen.
        void clear(void);
        /// serialisiert die Daten.
        void serialize(Serializer* ser) const;

        /// Zuweisungsoperator
        LobbyServerInfo& operator= (const LobbyServerInfo& info)
        {
            serverid = info.serverid;
            name = info.name;
            host = info.host;
            port = info.port;
            version = info.version;
            ping = info.ping;
            map = info.map;
            curplayers = info.curplayers;
            maxplayers = info.maxplayers;
            has_password = info.has_password;

            return *this;
        }

        unsigned int getId(void) const { return serverid; }
        std::string getName(void) const { return name; }
        std::string getHost(void) const { return host; }
        unsigned short getPort(void) const { return port; }
        std::string getVersion(void) const { return version; }
        unsigned int getPing(void) const { return ping; }
        std::string getMap(void) const { return map; }
        unsigned int getCurPlayers(void) const { return curplayers; }
        unsigned int getMaxPlayers(void) const { return maxplayers; }
        bool hasPassword(void) const { return has_password; }

        void setId(const unsigned int serverid) { this->serverid = serverid; }
        void setName(const std::string name) { this->name = name; }
        void setHost(const std::string host) { this->host = host; }
        void setPort(const unsigned short port) { this->port = port; }
        void setVersion(const std::string version) { this->version = version; }
        void setPing(const unsigned short ping) { this->ping = ping; }
        void setMap(const std::string map) { this->map = map; }
        void setCurPlayers(const unsigned int curplayers) { this->curplayers = curplayers; }
        void setMaxPlayers(const unsigned int maxplayers) { this->maxplayers = maxplayers; }
        void setPassword(bool has_password) { this->has_password = has_password; }

    private:
        unsigned int serverid;
        std::string name;
        std::string host;
        unsigned short port;
        std::string version;
        unsigned int ping;
        std::string map;
        unsigned int curplayers;
        unsigned int maxplayers;
        bool has_password;
};

#endif // LOBBYSERVERINFO_H_INCLUDED
