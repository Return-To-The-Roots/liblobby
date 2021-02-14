//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/LobbyInterface.h"

#include <mygettext/mygettext.h>

void LobbyInterface::LC_LoggedIn(const std::string& /*email*/) {}
void LobbyInterface::LC_Connected() {}
void LobbyInterface::LC_Created() {}

void LobbyInterface::LC_Status_Waiting() {}
void LobbyInterface::LC_Status_Error(const std::string& /*error*/) {}

void LobbyInterface::LC_Chat(const std::string& /*player*/, const std::string& /*text*/) {}

/**
 *  Status: Verbindung verloren.
 */
void LobbyInterface::LC_Status_ConnectionLost()
{
    LC_Status_Error(_("Connection to Host closed!"));
}

/**
 *  Status: fehlerhafte Anfrage / kaputte Daten.
 */
void LobbyInterface::LC_Status_IncompleteMessage()
{
    LC_Status_Error(_("Too short Message received!"));
}
