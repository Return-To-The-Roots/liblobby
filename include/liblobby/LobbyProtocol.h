//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyProtocol_h
#define liblobby_include_liblobby_LobbyProtocol_h

#include <cstdint>

const unsigned LOBBYPROTOCOL_VERSION = 0xFF0007FF;

namespace lobbyprotocol {

inline uint32_t extractVersion(uint32_t version)
{
    if((version & 0xFF0000FF) == 0xFF0000FF)
    {
        return (version >> 8) & 0xFFFF;
    }

    return 0;
}

} // namespace lobbyprotocol

enum
{
    NMS_LOBBY_LOGIN = 0x1000,
    NMS_LOBBY_LOGIN_DONE,
    NMS_LOBBY_LOGIN_ERROR,

    // deprecated
    // NMS_LOBBY_REGISTER = 0x2000,
    // NMS_LOBBY_REGISTER_DONE,
    // NMS_LOBBY_REGISTER_ERROR,

    NMS_LOBBY_SERVERLIST = 0x3000,
    NMS_LOBBY_SERVERINFO,
    NMS_LOBBY_RANKINGLIST,
    NMS_LOBBY_PLAYERLIST,

    NMS_LOBBY_UNUSED = 0x4000,
    NMS_LOBBY_ID,
    NMS_LOBBY_CHAT,

    NMS_LOBBY_PING = 0x5000,
    NMS_LOBBY_PONG,

    NMS_LOBBY_SERVER_ADD = 0x6000,
    NMS_LOBBY_SERVER_ADD_FAILED,
    NMS_LOBBY_SERVER_DELETE,
    NMS_LOBBY_SERVER_UPDATE_PLAYER,
    NMS_LOBBY_SERVER_UPDATE_MAP,
    NMS_LOBBY_SERVER_JOIN,

    NMS_LOBBY_RANKING_INFO = 0x7000
};

#endif // liblobby_include_liblobby_LobbyProtocol_h
