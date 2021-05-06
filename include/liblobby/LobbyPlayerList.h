//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include "LobbyPlayerInfo.h"

#include <s25util/SerializableArray.h>

class LobbyPlayerList : public SerializableArray<LobbyPlayerInfo>
{};
