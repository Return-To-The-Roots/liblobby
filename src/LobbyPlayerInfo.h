// $Id: LobbyPlayerInfo.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
	LobbyPlayerInfo(const unsigned playerid, Serializer * ser);
	/// leert die Informationen.
	void clear(void);
	/// serialisiert die Daten.
	void serialize(Serializer *ser) const;
	void deserialize(Serializer *ser);

	/// Zuweisungsoperator
	LobbyPlayerInfo& operator= (const LobbyPlayerInfo &info)
	{
		playerid = info.playerid;
		name = info.name;
		version = info.version;
		punkte = info.punkte;
		gewonnen = info.gewonnen;
		verloren = info.verloren;

		return *this;
	}

	unsigned int getId(void) const { return playerid; }
	std::string getName(void) const { return name; }
	std::string getVersion(void) const { return version; }
	int getPunkte(void) const { return punkte; }
	unsigned int getGewonnen(void) const { return gewonnen; }
	unsigned int getVerloren(void) const { return verloren; }

	void setId(const unsigned int playerid) { this->playerid = playerid; }
	void setName(const std::string name) { this->name = name; }
	void setVersion(const std::string version) { this->version = version; }
	void setPunkte(const unsigned int punkte) { this->punkte = punkte; }
	void setGewonnen(const unsigned int gewonnen) { this->gewonnen = gewonnen; }
	void setVerloren(const unsigned int verloren) { this->verloren = verloren; }

protected:
	unsigned int playerid;
	std::string name;
	std::string version;
	int punkte;
	unsigned int gewonnen;
	unsigned int verloren;
};

#endif // LOBBYPLAYERINFO_H_INCLUDED
