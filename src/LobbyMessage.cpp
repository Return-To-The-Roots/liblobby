//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/LobbyMessage.h"
#include "liblobby/LobbyMessages.h"
#include "liblobby/LobbyProtocol.h"

Message* LobbyMessage::create_lobby(unsigned short id)
{
    Message* msg = nullptr;

    switch(id)
    {
        default: return create_base(id);

        case NMS_LOBBY_LOGIN: msg = new LobbyMessage_Login(); break;
        case NMS_LOBBY_LOGIN_DONE: msg = new LobbyMessage_Login_Done(); break;
        case NMS_LOBBY_LOGIN_ERROR: msg = new LobbyMessage_Login_Error(); break;

        case NMS_LOBBY_SERVERLIST: msg = new LobbyMessage_ServerList(); break;
        case NMS_LOBBY_SERVERINFO: msg = new LobbyMessage_ServerInfo(); break;
        case NMS_LOBBY_PLAYERLIST: msg = new LobbyMessage_PlayerList(); break;

        case NMS_LOBBY_ID: msg = new LobbyMessage_Id(); break;
        case NMS_LOBBY_CHAT: msg = new LobbyMessage_Chat(); break;

        case NMS_LOBBY_PING: msg = new LobbyMessage_Ping(); break;
        case NMS_LOBBY_PONG: msg = new LobbyMessage_Pong(); break;

        case NMS_LOBBY_SERVER_ADD: msg = new LobbyMessage_Server_Add(); break;
        case NMS_LOBBY_SERVER_ADD_FAILED: msg = new LobbyMessage_Server_Add_Failed(); break;
        case NMS_LOBBY_SERVER_DELETE: msg = new LobbyMessage_Server_Delete(); break;
        case NMS_LOBBY_SERVER_UPDATE_PLAYER: msg = new LobbyMessage_Server_Update_Player(); break;
        case NMS_LOBBY_SERVER_UPDATE_MAP: msg = new LobbyMessage_Server_Update_Map(); break;
        case NMS_LOBBY_SERVER_JOIN: msg = new LobbyMessage_Server_Join(); break;
    }

    return msg;
}
