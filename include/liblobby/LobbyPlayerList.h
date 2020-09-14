//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include "LobbyPlayerInfo.h"

#include <s25util/SerializableArray.h>

class LobbyPlayerList : public SerializableArray<LobbyPlayerInfo>
{};
