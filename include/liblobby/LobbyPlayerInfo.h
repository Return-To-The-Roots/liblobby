//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once

#include <string>

class Serializer;

class LobbyPlayerInfo
{
public:
    LobbyPlayerInfo();
    LobbyPlayerInfo(unsigned playerId, Serializer& ser);
    /// leert die Informationen.
    void clear();
    /// serialisiert die Daten.
    void serialize(Serializer& ser) const;
    void deserialize(Serializer& ser);

    unsigned getId() const { return playerId_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getVersion() const { return version_; }

    void setId(const unsigned playerId) { this->playerId_ = playerId; }
    void setName(const std::string& name) { this->name_ = name; }
    void setEmail(const std::string& email) { this->email_ = email; }
    void setVersion(const std::string& version) { this->version_ = version; }

    /// Is the player in a game (not synchronized ATM)
    bool isIngame;

protected:
    unsigned playerId_;
    std::string name_;
    std::string email_;
    std::string version_;
};
