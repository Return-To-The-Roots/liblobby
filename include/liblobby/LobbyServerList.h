//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyServerList_h
#define liblobby_include_liblobby_LobbyServerList_h

#include "LobbyServerInfo.h"

#include <libutil/SerializableArray.h>

class LobbyServerList : public SerializableArray<LobbyServerInfo>
{};

#endif // liblobby_include_liblobby_LobbyServerList_h
