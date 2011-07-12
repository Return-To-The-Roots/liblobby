// $Id: MySQL.h 7292 2011-07-12 12:48:19Z FloSoft $
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
#ifndef MYSQL_H_INCLUDED
#define MYSQL_H_INCLUDED

#pragma once

#include <mysql/mysql.h>

#include "Singleton.h"

#include "LobbyServerList.h"
#include "LobbyServerInfo.h"
#include "LobbyPlayerList.h"

class MySQL : public Singleton<MySQL>
{
public:
  MySQL(void);
  ~MySQL(void);

  bool Connect(std::string host = "", std::string user = "", std::string pass = "", std::string db = "");
  void Disconnect(void);

  bool LoginUser(const std::string &user, const std::string &pass, std::string &email);
  bool RegisterUser(const std::string &user, const std::string &pass, const std::string &email);
  bool GetServerList(LobbyServerList *List);
  bool GetServerInfo(unsigned int id, LobbyServerInfo *Info);
  bool GetRankingList(LobbyPlayerList* List);
  bool GetRankingInfo(LobbyPlayerInfo& player);

  bool AddServer(LobbyServerInfo *Info);
  bool DeleteServer(unsigned int id);
  bool UpdateServer(unsigned int id, const std::string &map);
  bool UpdateServerPC(unsigned int id, unsigned int curplayer, unsigned int maxplayer);
  bool UpdateServerPing(unsigned int id, unsigned int ping);

private:
  bool DoQuery(std::string query);

private:
  MYSQL *m_pMySQL;
  struct {
	  std::string host;
	  std::string user;
	  std::string pass;
	  std::string db;
  } m_Connection;
};

#define MYSQLCLIENT MySQL::inst()

#endif // MYSQL_H_INCLUDED
