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
    playerId_ = 0;
    name_.clear();
    version_.clear();
    punkte_ = 0;
    gewonnen_ = 0;
    verloren_ = 0;
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
    ser.PushSignedInt(punkte_);
    ser.PushUnsignedInt(gewonnen_);
    ser.PushUnsignedInt(verloren_);
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
    punkte_ = ser.PopSignedInt();
    gewonnen_ = ser.PopUnsignedInt();
    verloren_ = ser.PopUnsignedInt();
}
