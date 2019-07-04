// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
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
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "liblobby/LobbyMessage.h"
#include "liblobby/LobbyMessages.h"
#include "liblobby/LobbyProtocol.h"

Message* LobbyMessage::create_lobby(unsigned short id)
{
    Message* msg = nullptr;

    switch(id)
    {
        default: return create_base(id);

        case NMS_LOBBY_LOGIN: { msg = new LobbyMessage_Login();
        }
        break;
        case NMS_LOBBY_LOGIN_DONE: { msg = new LobbyMessage_Login_Done();
        }
        break;
        case NMS_LOBBY_LOGIN_ERROR: { msg = new LobbyMessage_Login_Error();
        }
        break;

        case NMS_LOBBY_SERVERLIST: { msg = new LobbyMessage_ServerList();
        }
        break;
        case NMS_LOBBY_SERVERINFO: { msg = new LobbyMessage_ServerInfo();
        }
        break;
        case NMS_LOBBY_RANKINGLIST: { msg = new LobbyMessage_RankingList();
        }
        break;
        case NMS_LOBBY_PLAYERLIST: { msg = new LobbyMessage_PlayerList();
        }
        break;

        case NMS_LOBBY_ID: { msg = new LobbyMessage_Id();
        }
        break;
        case NMS_LOBBY_CHAT: { msg = new LobbyMessage_Chat();
        }
        break;

        case NMS_LOBBY_PING: { msg = new LobbyMessage_Ping();
        }
        break;
        case NMS_LOBBY_PONG: { msg = new LobbyMessage_Pong();
        }
        break;

        case NMS_LOBBY_SERVER_ADD: { msg = new LobbyMessage_Server_Add();
        }
        break;
        case NMS_LOBBY_SERVER_ADD_FAILED: { msg = new LobbyMessage_Server_Add_Failed();
        }
        break;
        case NMS_LOBBY_SERVER_DELETE: { msg = new LobbyMessage_Server_Delete();
        }
        break;
        case NMS_LOBBY_SERVER_UPDATE_PLAYER: { msg = new LobbyMessage_Server_Update_Player();
        }
        break;
        case NMS_LOBBY_SERVER_UPDATE_MAP: { msg = new LobbyMessage_Server_Update_Map();
        }
        break;
        case NMS_LOBBY_SERVER_JOIN: { msg = new LobbyMessage_Server_Join();
        }
        break;

        case NMS_LOBBY_RANKING_INFO: { msg = new LobbyMessage_Lobby_Ranking_Info();
        }
        break;
    }

    return msg;
}
