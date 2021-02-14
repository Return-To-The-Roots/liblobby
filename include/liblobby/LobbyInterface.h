//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include <string>

class LobbyPlayerInfo;
class LobbyServerList;
class LobbyServerInfo;
class LobbyPlayerList;

class LobbyInterface
{
public:
    virtual ~LobbyInterface() = default;

    virtual void LC_LoggedIn(const std::string& email);
    virtual void LC_Connected();
    virtual void LC_Created();

    virtual void LC_Status_ConnectionLost();
    virtual void LC_Status_IncompleteMessage();
    virtual void LC_Status_Waiting();
    virtual void LC_Status_Error(const std::string& error);

    virtual void LC_Chat(const std::string& player, const std::string& text);
    virtual void LC_ServerList(const LobbyServerList& /*servers*/) {}
    virtual void LC_ServerInfo(const LobbyServerInfo& /*info*/) {}
    virtual void LC_PlayerList(const LobbyPlayerList& /*players*/) {}
};
