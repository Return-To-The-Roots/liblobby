//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/LobbyServerInfo.h"

#include <s25util/Serializer.h>

LobbyServerInfo::LobbyServerInfo()
{
    clear();
}

LobbyServerInfo::LobbyServerInfo(const unsigned /*playerId*/, Serializer& ser)
    : serverid(ser.PopUnsignedInt()), name(ser.PopLongString()), host(ser.PopLongString()), port(ser.PopUnsignedShort()),
      version(ser.PopLongString()), ping(ser.PopUnsignedInt()), map(ser.PopLongString()), curplayers(ser.PopUnsignedInt()),
      maxplayers(ser.PopUnsignedInt()), has_password(ser.PopBool())
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
    ser.PushUnsignedInt(serverid);
    ser.PushLongString(name);
    ser.PushLongString(host);
    ser.PushUnsignedShort(port);
    ser.PushLongString(version);
    ser.PushUnsignedInt(ping);
    ser.PushLongString(map);
    ser.PushUnsignedInt(curplayers);
    ser.PushUnsignedInt(maxplayers);
    ser.PushBool(has_password);
}
