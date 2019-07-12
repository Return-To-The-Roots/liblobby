//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//

#pragma once
#ifndef liblobby_include_liblobby_LobbyPlayerInfo_h
#define liblobby_include_liblobby_LobbyPlayerInfo_h

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
    std::string getName() const { return name_; }
    std::string getEmail() const { return email_; }
    std::string getVersion() const { return version_; }
    int getPunkte() const { return punkte_; }
    unsigned getGewonnen() const { return gewonnen_; }
    unsigned getVerloren() const { return verloren_; }

    void setId(const unsigned playerId) { this->playerId_ = playerId; }
    void setName(const std::string& name) { this->name_ = name; }
    void setEmail(const std::string& email) { this->email_ = email; }
    void setVersion(const std::string& version) { this->version_ = version; }
    void setPunkte(const unsigned punkte) { this->punkte_ = punkte; }
    void setGewonnen(const unsigned gewonnen) { this->gewonnen_ = gewonnen; }
    void setVerloren(const unsigned verloren) { this->verloren_ = verloren; }

    /// Is the player in a game (not synchronized ATM)
    bool isIngame;

protected:
    unsigned playerId_;
    std::string name_;
    std::string email_;
    std::string version_;
    int punkte_;
    unsigned gewonnen_;
    unsigned verloren_;
};

#endif // !liblobby_include_liblobby_LobbyPlayerInfo_h
