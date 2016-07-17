// Copyright (c) 2016 Settlers Freaks (sf-team at siedler25.org)
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

#include "main.h" // IWYU pragma: keep
#include "LobbyServer.h"
#include "libutil/src/Socket.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
    if(argc != 6)
    {
        std::cerr << "Wrong number of arguments: port host user pass dbName";
        return 1;
    }
    if(!Socket::Initialize())
        return 2;
    unsigned port = atoi(argv[1]);
    std::string mysqlHost = argv[2];
    std::string mysqlUser = argv[3];
    std::string mysqlPass = argv[4];
    std::string mysqlDb = argv[5];
    LOBBYSERVER.Start(port, mysqlHost, mysqlUser, mysqlPass, mysqlDb);
    while(LOBBYSERVER.Run() == 0)
        ;
    // Dummy test only! Hence we quit again
    LOBBYSERVER.Stop();
    return 0;
}