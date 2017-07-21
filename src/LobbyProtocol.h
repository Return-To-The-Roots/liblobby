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
#ifndef LOBBYPROTOCOL_H_INCLUDED
#define LOBBYPROTOCOL_H_INCLUDED

#pragma once

const unsigned LOBBYPROTOCOL_VERSION = 0xFF0006FF;

enum
{
    NMS_LOBBY_LOGIN = 0x1000,
    NMS_LOBBY_LOGIN_DONE,
    NMS_LOBBY_LOGIN_ERROR,

    NMS_LOBBY_REGISTER = 0x2000,
    NMS_LOBBY_REGISTER_DONE,
    NMS_LOBBY_REGISTER_ERROR,

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

#endif // LOBBYPROTOCOL_H_INCLUDED
