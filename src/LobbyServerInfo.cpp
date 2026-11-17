//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/LobbyServerInfo.h"

#include <s25util/Serializer.h>

LobbyServerInfo::LobbyServerInfo()
{
    clear();
}

LobbyServerInfo::LobbyServerInfo(const unsigned /*playerId*/, Serializer& ser)
    : serverid(ser.popUnsignedInt()), name(ser.popLongString()), host(ser.popLongString()),
      port(ser.popUnsignedShort()), version(ser.popLongString()), ping(ser.popUnsignedInt()), map(ser.popLongString()),
      curplayers(ser.popUnsignedInt()), maxplayers(ser.popUnsignedInt()), has_password(ser.popBool())
{}

/**
 *  leert die Informationen.
 */
void LobbyServerInfo::clear()
{
    serverid = 0;
    name.clear();
    host.clear();
    port = 0;
    version.clear();
    ping = 0;
    map.clear();
    curplayers = 0;
    maxplayers = 0;
    has_password = false;
}

/**
 *  serialisiert die Daten.
 *
 *  @param[in,out] data Datensatz, muss groß genug sein
 *
 *  @return liefert die Größe der Daten zurück.
 */
void LobbyServerInfo::serialize(Serializer& ser) const
{
    ser.pushUnsignedInt(serverid);
    ser.pushLongString(name);
    ser.pushLongString(host);
    ser.pushUnsignedShort(port);
    ser.pushLongString(version);
    ser.pushUnsignedInt(ping);
    ser.pushLongString(map);
    ser.pushUnsignedInt(curplayers);
    ser.pushUnsignedInt(maxplayers);
    ser.pushBool(has_password);
}
