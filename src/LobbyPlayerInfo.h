// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef LOBBYPLAYERINFO_H_INCLUDED
#define LOBBYPLAYERINFO_H_INCLUDED

#pragma once

class Serializer;

class LobbyPlayerInfo
{
    public:
        /// Konstruktor von @p LobbyPlayerInfo.
        LobbyPlayerInfo(void);
        /// Deserialisierungs-Konstruktor von @p LobbyPlayerInfo.
        LobbyPlayerInfo(const unsigned playerid, Serializer& ser);
        /// leert die Informationen.
        void clear(void);
        /// serialisiert die Daten.
        void serialize(Serializer& ser) const;
        void deserialize(Serializer& ser);

        unsigned int getId(void) const { return playerid_; }
        std::string getName(void) const { return name_; }
        std::string getEmail(void) const { return email_; }
        std::string getVersion(void) const { return version_; }
        int getPunkte(void) const { return punkte_; }
        unsigned int getGewonnen(void) const { return gewonnen_; }
        unsigned int getVerloren(void) const { return verloren_; }

        void setId(const unsigned int playerid) { this->playerid_ = playerid; }
        void setName(const std::string& name) { this->name_ = name; }
        void setEmail(const std::string& email) { this->email_ = email; }
        void setVersion(const std::string& version) { this->version_ = version; }
        void setPunkte(const unsigned int punkte) { this->punkte_ = punkte; }
        void setGewonnen(const unsigned int gewonnen) { this->gewonnen_ = gewonnen; }
        void setVerloren(const unsigned int verloren) { this->verloren_ = verloren; }

    protected:
        unsigned int playerid_;
        std::string name_;
        std::string email_;
        std::string version_;
        int punkte_;
        unsigned int gewonnen_;
        unsigned int verloren_;
};

#endif // LOBBYPLAYERINFO_H_INCLUDED
