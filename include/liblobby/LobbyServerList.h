//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include "LobbyServerInfo.h"

#include <s25util/SerializableArray.h>

class LobbyServerList : public SerializableArray<LobbyServerInfo>
{};
