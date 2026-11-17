//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
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
    ser.pushUnsignedInt(playerId_);
    ser.pushLongString(name_);
    ser.pushLongString(version_);
    ser.pushSignedInt(0);   // points
    ser.pushUnsignedInt(0); // games won
    ser.pushUnsignedInt(0); // games lost
}

/**
 *  deserialisiert die Daten.
 *
 *  @param[in] ser der benutzte Serializer
 */
void LobbyPlayerInfo::deserialize(Serializer& ser)
{
    playerId_ = ser.popUnsignedInt();
    name_ = ser.popLongString();
    version_ = ser.popLongString();
    ser.popSignedInt();   // points
    ser.popUnsignedInt(); // games won
    ser.popUnsignedInt(); // games lost
}
