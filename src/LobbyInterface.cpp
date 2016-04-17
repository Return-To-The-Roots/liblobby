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

#include "main.h" // IWYU pragma: keep
#include "LobbyInterface.h"

// Include last!
#include "DebugNew.h" // IWYU pragma: keep
#include "mygettext.h"

void LobbyInterface::LC_LoggedIn(const std::string&  /*email*/) { }
void LobbyInterface::LC_Registered() { }
void LobbyInterface::LC_Connected() { }
void LobbyInterface::LC_Created() { }
void LobbyInterface::LC_RankingInfo(const LobbyPlayerInfo&  /*player*/) { }

void LobbyInterface::LC_Status_Waiting() { }
void LobbyInterface::LC_Status_Error(const std::string&  /*error*/) { }

void LobbyInterface::LC_Chat(const std::string&  /*player*/, const std::string&  /*text*/) { }

/**
 *  Status: Verbindung verloren.
 *
 *  @author FloSoft
 */
void LobbyInterface::LC_Status_ConnectionLost()
{
    LC_Status_Error(_("Connection to Host closed!"));
}

/**
 *  Status: fehlerhafte Anfrage / kaputte Daten.
 *
 *  @author FloSoft
 */
void LobbyInterface::LC_Status_IncompleteMessage()
{
    LC_Status_Error(_("Too short Message received!"));
}
