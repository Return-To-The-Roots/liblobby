// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "LobbyServerInfo.h"
#include "libutil/Serializer.h"

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
