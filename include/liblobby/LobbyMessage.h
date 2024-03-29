//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include <s25util/Message.h>

class LobbyMessage : public Message
{
public:
    LobbyMessage(unsigned short id) : Message(id) {}

    static Message* create_lobby(unsigned short id);

    Message* create(unsigned short id) const override { return create_lobby(id); }
};
