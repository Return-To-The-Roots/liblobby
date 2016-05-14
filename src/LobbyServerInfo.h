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
#include <string>

class LobbyServerInfo
{
    public:
        LobbyServerInfo();
        LobbyServerInfo(const unsigned playerId, Serializer& ser);
        /// leert die Informationen.
        void clear();
        /// serialisiert die Daten.
        void serialize(Serializer& ser) const;

        unsigned int getId() const { return serverid; }
        std::string getName() const { return name; }
        std::string getHost() const { return host; }
        unsigned short getPort() const { return port; }
        std::string getVersion() const { return version; }
        unsigned int getPing() const { return ping; }
        std::string getMap() const { return map; }
        unsigned int getCurPlayers() const { return curplayers; }
        unsigned int getMaxPlayers() const { return maxplayers; }
        bool hasPassword() const { return has_password; }

        void setId(const unsigned int serverid) { this->serverid = serverid; }
        void setName(const std::string& name) { this->name = name; }
        void setHost(const std::string& host) { this->host = host; }
        void setPort(const unsigned short port) { this->port = port; }
        void setVersion(const std::string& version) { this->version = version; }
        void setPing(const unsigned short ping) { this->ping = ping; }
        void setMap(const std::string& map) { this->map = map; }
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
