//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#include "liblobby/LobbyPlayerInfo.h"

#include <s25util/Serializer.h>

LobbyPlayerInfo::LobbyPlayerInfo()
{
    clear();
}

LobbyPlayerInfo::LobbyPlayerInfo(const unsigned /*playerId*/, Serializer& ser)
{
    deserialize(ser);
    isIngame = false;
}

/**
 *  leert die Informationen.
 */
void LobbyPlayerInfo::clear()
{
    playerId_ = static_cast<unsigned>(-1);
    name_.clear();
    version_.clear();
    isIngame = false;
}

/**
 *  serialisiert die Daten.
 *
 *  @param[in] ser der benutzte Serializer
 */
void LobbyPlayerInfo::serialize(Serializer& ser) const
{
    ser.PushUnsignedInt(playerId_);
    ser.PushLongString(name_);
    ser.PushLongString(version_);
    ser.PushSignedInt(0);   // points
    ser.PushUnsignedInt(0); // games won
    ser.PushUnsignedInt(0); // games lost
}

/**
 *  deserialisiert die Daten.
 *
 *  @param[in] ser der benutzte Serializer
 */
void LobbyPlayerInfo::deserialize(Serializer& ser)
{
    playerId_ = ser.PopUnsignedInt();
    name_ = ser.PopLongString();
    version_ = ser.PopLongString();
    ser.PopSignedInt();   // points
    ser.PopUnsignedInt(); // games won
    ser.PopUnsignedInt(); // games lost
}
