//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyServerInfo_h
#define liblobby_include_liblobby_LobbyServerInfo_h

#include <string>

class Serializer;

class LobbyServerInfo
{
public:
    LobbyServerInfo();
    LobbyServerInfo(unsigned playerId, Serializer& ser);
    /// leert die Informationen.
    void clear();
    /// serialisiert die Daten.
    void serialize(Serializer& ser) const;

    unsigned getId() const { return serverid; }
    std::string getName() const { return name; }
    std::string getHost() const { return host; }
    unsigned short getPort() const { return port; }
    std::string getVersion() const { return version; }
    unsigned getPing() const { return ping; }
    std::string getMap() const { return map; }
    unsigned getCurPlayers() const { return curplayers; }
    unsigned getMaxPlayers() const { return maxplayers; }
    bool hasPassword() const { return has_password; }

    void setId(const unsigned serverid) { this->serverid = serverid; }
    void setName(const std::string& name) { this->name = name; }
    void setHost(const std::string& host) { this->host = host; }
    void setPort(const unsigned short port) { this->port = port; }
    void setVersion(const std::string& version) { this->version = version; }
    void setPing(const unsigned short ping) { this->ping = ping; }
    void setMap(const std::string& map) { this->map = map; }
    void setCurPlayers(const unsigned curplayers) { this->curplayers = curplayers; }
    void setMaxPlayers(const unsigned maxplayers) { this->maxplayers = maxplayers; }
    void setPassword(bool has_password) { this->has_password = has_password; }

private:
    unsigned serverid;
    std::string name;
    std::string host;
    unsigned short port;
    std::string version;
    unsigned ping;
    std::string map;
    unsigned curplayers;
    unsigned maxplayers;
    bool has_password;
};

#endif // !liblobby_include_liblobby_LobbyServerInfo_h
