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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "libLobbyDefines.h" // IWYU pragma: keep
#include "MySQL.h"
#include "libutil/src/Log.h"
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Konstruktor
MySQL::MySQL(void)
{
    m_pMySQL = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Destruktor
MySQL::~MySQL(void)
{
    Disconnect();
}

///////////////////////////////////////////////////////////////////////////////
// Verbindung zum Server trennen
void MySQL::Disconnect(void)
{
    if(m_pMySQL)
        mysql_close(m_pMySQL);
    m_pMySQL = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Zum Server verbinden
bool MySQL::Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
{
    if(m_Connection.host.empty() && !host.empty())
    {
        m_Connection.host = host;
        m_Connection.user = user;
        m_Connection.pass = pass;
        m_Connection.db = db;
    }
    if(m_Connection.host.empty())
        return false;

    bool test = false;

    if(m_pMySQL == NULL)
    {
        m_pMySQL = mysql_init(NULL);
        test = true;
    }

    if(m_pMySQL == NULL)
        return false;

    if(test || mysql_ping(m_pMySQL) != 0)
    {
        if(mysql_real_connect(m_pMySQL, m_Connection.host.c_str(), m_Connection.user.c_str(), m_Connection.pass.c_str(), m_Connection.db.c_str(), MYSQL_PORT, NULL, 0) == NULL)
        {
            LOG.write("Failed to connect to database: Error: %s\n") % mysql_error(m_pMySQL);
            return false;
        }
        if(mysql_select_db(m_pMySQL, m_Connection.db.c_str() ) < 0 )
        {
            LOG.write("Can't select the %s database: %s\n") % m_Connection.db % mysql_error(m_pMySQL);
            return false;
        }
    }

    if(!host.empty())
        mysql_query(m_pMySQL, "DELETE FROM `lobby_servers`;");

    return true;
}

bool MySQL::DoQuery(const std::string& query)
{
    if(!Connect())
        return false;

    if(mysql_query(m_pMySQL, query.c_str()))
    {
        LOG.write("Failed to send query to database: Error: %s\n") % mysql_error(m_pMySQL);
        return false;
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Benutzer "einloggen"
bool MySQL::LoginUser(const std::string& user, const std::string& pass, std::string& email, const std::string& ip)
{
    char user2[256], pass2[256];
    mysql_real_escape_string(m_pMySQL, user2, user.c_str(), (unsigned long)user.length());
    mysql_real_escape_string(m_pMySQL, pass2, pass.c_str(), (unsigned long)pass.length());

    char query[1024];
    //snprintf(query, 1024, "SELECT * FROM `lobby_users` WHERE `user` = '%s' AND `pass` = DES_ENCRYPT('%s', '%s') AND `email` IS NOT NULL LIMIT 1;", user2, pass2, user2);
    //snprintf(query, 1024, "SELECT `username`,`useremail` FROM `tb_user` WHERE `username` = '%s' AND `userpassword` = MD5('%s') AND `userbanned` = 0 AND `useremail` IS NOT NULL LIMIT 1;", user2, pass2);
    snprintf(query, 1024, "SELECT `user`,`mail`,id FROM `users` WHERE `user` = '%s' AND `pass` = MD5('%s') AND `mail` IS NOT NULL AND login_allowed = 1 AND banned = 0 LIMIT 1;", user2, pass2);

    // LOG.write(("%s\n", query);

    if(!DoQuery(query))
        return false;

    MYSQL_RES* pResult = mysql_store_result(m_pMySQL);

    int iRows = (int)mysql_num_rows(pResult);

    if(iRows == 0)
    {
        mysql_free_result(pResult);
        return false;
    }

    MYSQL_ROW Row = mysql_fetch_row(pResult);

    // LOG.write(("%s %s %s %s\n", Row[0], Row[1], Row[2], Row[3]);

    if( (strcmp(Row[0], user2) == 0) && Row[1] )
    {
        email = Row[1];

        // save login
        snprintf(query, 1024, "INSERT INTO `users_online` (`uid`, `time`, `ip`, `from`) VALUES(%s, UNIX_TIMESTAMP(), '%s', 'rttr');", Row[2], ip.c_str());
        DoQuery(query);

        // delete entries older than 30 days
        snprintf(query, 1024, "DELETE FROM `users_online` WHERE `uid` = %s AND `time` < UNIX_TIMESTAMP()-%d;", Row[2], 3600 * 24 * 30);
        DoQuery(query);

        mysql_free_result(pResult);
        return true;
    }
    mysql_free_result(pResult);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Benutzer registrieren
bool MySQL::RegisterUser(const std::string& user, const std::string& pass, const std::string& email)
{
    MYSQL_RES*   pResult;
    char query[1024];
    char user2[256], pass2[256], email2[256];
    mysql_real_escape_string(m_pMySQL, user2, user.c_str(), (unsigned long)user.length());
    mysql_real_escape_string(m_pMySQL, pass2, pass.c_str(), (unsigned long)pass.length());
    mysql_real_escape_string(m_pMySQL, email2, email.c_str(), (unsigned long)email.length());

    //snprintf(query, 1024, "SELECT * FROM `lobby_users` WHERE `user` = '%s' LIMIT 1;", user2);

    // not implemented
    return false;

    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    int iRows = (int)mysql_num_rows(pResult);

    if(iRows != 0)
    {
        LOG.write("User %s already exist (%d %d rows) / %s!\n") % user % iRows % pResult->row_count % mysql_error(m_pMySQL);
        mysql_free_result(pResult);
        return false;
    }

    mysql_free_result(pResult);

    snprintf(query, 1024, "INSERT INTO `lobby_users` VALUES ( '', '%s', DES_ENCRYPT('%s', '%s'), '%s', '0', '0' )", user2, pass2, user2, email2);

    if(!DoQuery(query))
        return false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Serverliste abrufen
bool MySQL::GetServerList(LobbyServerList* List)
{
    MYSQL_RES*   pResult;
    MYSQL_ROW   Row;

    char query[1024];
    snprintf(query, 1024, "SELECT * FROM `lobby_servers` WHERE `curplayer` > 0 AND `maxplayers` > 0 ORDER BY `name` ASC;");

    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    unsigned int count = (unsigned int)mysql_num_rows(pResult);

    if(count == 0)
    {
        mysql_free_result(pResult);
        return true;
    }

    for(unsigned int i = 0; i < count; i++)
    {
        Row = mysql_fetch_row(pResult);

        LobbyServerInfo server;

        server.setId(atoi(Row[0]));
        server.setName(Row[1]);
        server.setHost(Row[2]);
        server.setPort(atoi(Row[3]));
        server.setVersion(Row[4]);
        server.setPing(atoi(Row[5]));
        server.setMap(Row[6]);
        server.setMaxPlayers(atoi(Row[7]));
        server.setCurPlayers(atoi(Row[8]));
        server.setPassword((atoi(Row[9]) != 0));

        List->push_back(server);
    }

    mysql_free_result(pResult);
    return true;
}

bool MySQL::GetServerInfo(unsigned int id, LobbyServerInfo* Info)
{
    MYSQL_RES*   pResult;
    MYSQL_ROW   Row;

    char query[1024];
    snprintf(query, 1024, "SELECT * FROM `lobby_servers` WHERE `id` = %u ORDER BY `name` ASC LIMIT 1;", id);

    //LOG.write(("%s\n", query);

    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    if(mysql_num_rows(pResult) == 0)
    {
        mysql_free_result(pResult);
        return true;
    }

    Row = mysql_fetch_row(pResult);
    Info->setId(atoi(Row[0]));
    Info->setName(Row[1]);
    Info->setHost(Row[2]);
    Info->setPort(atoi(Row[3]));
    Info->setVersion(Row[4]);
    Info->setPing(atoi(Row[5]));
    Info->setMap(Row[6]);

    if(Row[7])
        Info->setMaxPlayers(atoi(Row[7]));

    if(Row[8])
        Info->setCurPlayers(atoi(Row[8]));

    Info->setPassword((atoi(Row[9]) != 0));

    mysql_free_result(pResult);
    return true;
}

bool MySQL::GetRankingList(LobbyPlayerList* List)
{
    MYSQL_RES*   pResult;
    MYSQL_ROW   Row;

    char query[1024];
    //snprintf(query, 1024, "SELECT username, win, lose FROM `tb_user` WHERE `useremail` IS NOT NULL AND (`win` > 0 OR `lose` > 0) ORDER BY `win` DESC, `lose` ASC LIMIT 10;");
    snprintf(query, 1024, "SELECT user, win, lose FROM `users` WHERE `mail` IS NOT NULL AND (`win` > 0 OR `lose` > 0) ORDER BY `win` DESC, `lose` ASC LIMIT 10;");

    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    unsigned int count = (unsigned int)mysql_num_rows(pResult);

    if(count == 0)
    {
        mysql_free_result(pResult);
        return true;
    }

    for(unsigned int i = 0; i < count; i++)
    {
        Row = mysql_fetch_row(pResult);

        int punkte = atoi(Row[1]) * 100 - atoi(Row[2]) * 70;
        if(punkte < 0)
            punkte = 0;

        LobbyPlayerInfo player;
        player.setId(i + 1);
        player.setName(Row[0]);
        player.setGewonnen(atoi(Row[1]));
        player.setVerloren(atoi(Row[2]));
        player.setPunkte(punkte);

        List->push_back(player);
    }

    mysql_free_result(pResult);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *
 *
 *
 *  @author FloSoft
 */
bool MySQL::GetRankingInfo(LobbyPlayerInfo& player)
{
    MYSQL_RES*   pResult;
    MYSQL_ROW   Row;
    char query[1024];

    char name[256];
    mysql_real_escape_string(m_pMySQL, name, player.getName().c_str(), (unsigned long)player.getName().length());

    //snprintf(query, 1024, "SELECT username, win, lose FROM `tb_user` WHERE `username` = '%s' LIMIT 1;", name);
    snprintf(query, 1024, "SELECT user, win, lose FROM `users` WHERE `user` = '%s' LIMIT 1;", name);

    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    if(mysql_num_rows(pResult) == 0)
    {
        mysql_free_result(pResult);
        return true;
    }

    Row = mysql_fetch_row(pResult);

    int punkte = atoi(Row[1]) * 100 - atoi(Row[2]) * 70;
    if(punkte < 0)
        punkte = 0;

    //player.setId(0);
    player.setName(Row[0]);
    player.setGewonnen(atoi(Row[1]));
    player.setVerloren(atoi(Row[2]));
    player.setPunkte(punkte);

    mysql_free_result(pResult);

    return true;
}

bool MySQL::AddServer(LobbyServerInfo* Info)
{
    MYSQL_RES*   pResult;
    //MYSQL_ROW Row;

    char query[1024];

    char name[256];
    char host[256];
    char version[256];
    char map[256];
    mysql_real_escape_string(m_pMySQL, name, Info->getName().c_str(), (unsigned long)Info->getName().length());
    mysql_real_escape_string(m_pMySQL, host, Info->getHost().c_str(), (unsigned long)Info->getHost().length());
    mysql_real_escape_string(m_pMySQL, version, Info->getVersion().c_str(), (unsigned long)Info->getVersion().length());
    mysql_real_escape_string(m_pMySQL, map, Info->getMap().c_str(), (unsigned long)Info->getMap().length());

    snprintf(query, 1024, "SELECT * FROM `lobby_servers` WHERE `name` = '%s' LIMIT 1;", name);
    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);
    if(mysql_num_rows(pResult) != 0)
    {
        mysql_free_result(pResult);
        return false;
    }
    mysql_free_result(pResult);

    /*snprintf(query, 1024, "INSERT INTO `lobby_servers` (`name`) VALUES('new');" );
    if(!DoQuery(query))
        return false;

    snprintf(query, 1024, "SELECT * FROM `lobby_servers` WHERE `name` = 'new' LIMIT 1;");
    if(!DoQuery(query))
        return false;

    pResult = mysql_store_result(m_pMySQL);

    if(mysql_num_rows(pResult) == 0)
    {
        mysql_free_result(pResult);
        return false;
    }

    Row = mysql_fetch_row(pResult);

    mysql_free_result(pResult);*/

    snprintf(query, 1024, "INSERT INTO `lobby_servers` (`name`, `hostname`, `port`, `version`, `map`, `maxplayers`, `curplayer`, `has_password`) VALUES ('%s', '%s', '%d', '%s', '%s', 0, 0, %d);", name, host, Info->getPort(), version, map, (Info->hasPassword() ? 1 : 0));

    if(!DoQuery(query))
        return false;

    Info->setId((unsigned int)mysql_insert_id(m_pMySQL));

    LOG.write("Neuer Server erstellt: %d: %s!\n") % Info->getId() % Info->getName();
    return true;
}

bool MySQL::DeleteServer(unsigned int id)
{
    LobbyServerInfo Info;
    if(!GetServerInfo(id, &Info))
        return false;

    char query[1024];
    snprintf(query, 1024, "DELETE FROM `lobby_servers` WHERE `id` = %u LIMIT 1;", id);

    if(!DoQuery(query))
        return false;

    LOG.write("Server %s entfernt\n") % Info.getName();
    return true;
}

bool MySQL::UpdateServer(unsigned int id, const std::string& map)
{
    LobbyServerInfo Info;
    if(!GetServerInfo(id, &Info))
        return false;

    char map2[256];
    mysql_real_escape_string(m_pMySQL, map2, map.c_str(), (unsigned long)map.length());

    char query[1024];
    snprintf(query, 1024, "UPDATE `lobby_servers` SET `map` = '%s' WHERE `id` = '%u';", map2, id);

    if(!DoQuery(query))
        return false;

    LOG.write("Server %s aktualisiert: Karte: %s->%s\n") % Info.getName() % Info.getMap() % map;
    return true;
}

bool MySQL::UpdateServerPC(unsigned int id, unsigned int curplayer, unsigned int maxplayer)
{
    LobbyServerInfo Info;
    if(!GetServerInfo(id, &Info))
        return false;

    char query[1024];
    snprintf(query, 1024, "UPDATE `lobby_servers` SET `curplayer` = '%u', `maxplayers` = '%u' WHERE `id` = '%u';", curplayer, maxplayer, id);

    if(!DoQuery(query))
        return false;

    LOG.write("Server %s aktualisiert: Aktuelle Spielerzahl: %u/%u->%u/%u\n") % Info.getName() % Info.getCurPlayers() % Info.getMaxPlayers() % curplayer % maxplayer;
    return true;
}

bool MySQL::UpdateServerPing(unsigned int id, unsigned int ping)
{
    LobbyServerInfo Info;
    if(!GetServerInfo(id, &Info))
        return false;

    if(Info.getPing() == ping)
        return true;

    char query[1024];
    snprintf(query, 1024, "UPDATE `lobby_servers` SET `ping` = '%u' WHERE `id` = '%u';", ping, id);

    if(!DoQuery(query))
        return false;

    //LOG.write(("Server %s aktualisiert: Neuer Ping: %d->%d\n", Info.getName(), Info.getPing(), ping);
    return true;
}

bool MySQL::SetBan(const std::string& user, bool banned)
{
    char user2[256];
    mysql_real_escape_string(m_pMySQL, user2, user.c_str(), (unsigned long) user.length());

    char query[1024];
    snprintf(query, 1024, "UPDATE `users` SET `banned` = '%d' WHERE `user` = '%s';", banned ? 1 : 0, user2);

    if(!DoQuery(query))
        return false;

    return(true);
}

