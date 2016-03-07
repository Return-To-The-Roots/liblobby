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
#include "LobbyMessageInterface.h"

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Login-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyLogin(unsigned int  /*id*/, const unsigned int  /*revision*/, const std::string&  /*user*/, const std::string&  /*pass*/, const std::string&  /*version*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Login-Error-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyLoginError(unsigned int  /*id*/, const std::string&  /*error*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Logged-In-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyLoginDone(unsigned int  /*id*/, const std::string&  /*email*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Register-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyRegister(unsigned int  /*id*/, const unsigned int  /*revision*/, const std::string&  /*user*/, const std::string&  /*pass*/, const std::string&  /*email*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Register-Error-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyRegisterError(unsigned int  /*id*/, const std::string&  /*error*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Register-Done-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyRegisterDone(unsigned int  /*id*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Player-ID-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyID(unsigned int  /*id*/, const unsigned int&  /*playerid*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Chat-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyChat(unsigned int  /*id*/, const std::string&  /*player*/, const std::string&  /*text*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Ping-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyPing(unsigned int  /*id*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Pong-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyPong(unsigned int  /*id*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  ServerList-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerList(unsigned int  /*id*/)
{
}

void LobbyMessageInterface::OnNMSLobbyServerList(unsigned int  /*id*/, const LobbyServerList&  /*list*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  PlayerList-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyPlayerList(unsigned int  /*id*/)
{
}

void LobbyMessageInterface::OnNMSLobbyPlayerList(unsigned int  /*id*/, const LobbyPlayerList&  /*list*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  RankingList-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyRankingList(unsigned int  /*id*/)
{
}

void LobbyMessageInterface::OnNMSLobbyRankingList(unsigned int  /*id*/, const LobbyPlayerList&  /*list*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  ServerInfo-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerInfo(unsigned int  /*id*/, const unsigned int&  /*server*/)
{
}

void LobbyMessageInterface::OnNMSLobbyServerInfo(unsigned int  /*id*/, const LobbyServerInfo&  /*info*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Add-Done-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerAdd(unsigned int  /*id*/, const LobbyServerInfo&  /*info*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Add-Failed-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerAddFailed(unsigned int  /*id*/, const std::string&  /*error*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Update-Player-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerUpdatePlayer(unsigned int  /*id*/, const unsigned int  /*curplayer*/, const unsigned int  /*maxplayer*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Update-Map-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerUpdateMap(unsigned int  /*id*/, const std::string&  /*map*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Delete-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerDelete(unsigned int  /*id*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Server-Join-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyServerJoin(unsigned int  /*id*/)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lobby-Ranking-Info-Nachricht.
 *
 *  @author FloSoft
 */
void LobbyMessageInterface::OnNMSLobbyRankingInfo(unsigned int  /*id*/, const LobbyPlayerInfo&  /*player*/)
{
}
