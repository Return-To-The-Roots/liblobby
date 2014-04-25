// $Id: LobbyServerInfo.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#include "main.h"
#include "LobbyServerInfo.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/*
 *  Konstruktor von @p LobbyServerInfo.
 *
 *  @author FloSoft
 */
LobbyServerInfo::LobbyServerInfo(void)
{
    clear();
}

/// Deserialisierungs-Konstruktor von @p LobbyServerInfo.
LobbyServerInfo::LobbyServerInfo(const unsigned playerid, Serializer* ser) :
    serverid(ser->PopUnsignedInt()),
    name(ser->PopString()),
    host(ser->PopString()),
    port(ser->PopUnsignedShort()),
    version(ser->PopString()),
    ping(ser->PopUnsignedInt()),
    map(ser->PopString()),
    curplayers(ser->PopUnsignedInt()),
    maxplayers(ser->PopUnsignedInt()),
    has_password(ser->PopBool())
{

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  leert die Informationen.
 *
 *  @author FloSoft
 */
void LobbyServerInfo::clear(void)
{
    serverid = 0;
    name = "";
    host = "";
    version = "";
    ping = 0;
    map = "";
    curplayers = 0;
    maxplayers = 0;
    has_password = false;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  serialisiert die Daten.
 *
 *  @param[in,out] data Datensatz, muss groß genug sein
 *
 *  @return liefert die Größe der Daten zurück.
 *
 *  @author FloSoft
 */
void LobbyServerInfo::serialize(Serializer* ser) const
{
    ser->PushUnsignedInt(serverid);
    ser->PushString(name);
    ser->PushString(host);
    ser->PushUnsignedShort(port);
    ser->PushString(version);
    ser->PushUnsignedInt(ping);
    ser->PushString(map);
    ser->PushUnsignedInt(curplayers);
    ser->PushUnsignedInt(maxplayers);
    ser->PushBool(has_password);
}
