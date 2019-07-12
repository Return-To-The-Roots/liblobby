//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyMessage_h
#define liblobby_include_liblobby_LobbyMessage_h

#include <libutil/Message.h>

class LobbyMessage : public Message
{
public:
    LobbyMessage(unsigned short id)
        : Message(id)
    {}

    static
    Message* create_lobby(unsigned short id);

    Message* create(unsigned short id) const override
    {
        return create_lobby(id);
    }
};

#endif // !liblobby_include_liblobby_LobbyMessage_h
