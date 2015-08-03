################################################################################
### $Id: CMakeLists.txt 7521 2011-09-08 20:45:55Z FloSoft $
################################################################################

SET(SOURCES_LOBBY LobbyMessage.cpp          LobbyMessage.h
                  LobbyMessages.h
                  LobbyMessageInterface.cpp LobbyMessageInterface.h
                  LobbyInterface.cpp        LobbyInterface.h
                  LobbyPlayerInfo.cpp       LobbyPlayerInfo.h
                  LobbyPlayerList.h
                  LobbyServerInfo.cpp       LobbyServerInfo.h
                  LobbyServerList.h
   )

SET(SOURCES_LOBBY_C ${SOURCES_LOBBY} LobbyClient.cpp LobbyClient.h)
SET(SOURCES_LOBBY_S ${SOURCES_LOBBY} LobbyServer.cpp LobbyServer.h  LobbyPlayer.cpp LobbyPlayer.h MySQL.cpp MySQL.h)

################################################################################

INCLUDE_DIRECTORIES("../../"
                    "${PROJECT_SOURCE_DIR}/libutil/src/"
                    "${PROJECT_SOURCE_DIR}/mygettext/src/"
                   )

################################################################################

IF (LOBBY_C)

    ADD_LIBRARY(lobby_c STATIC ${SOURCES_LOBBY_C})
    TARGET_LINK_LIBRARIES(lobby_c s25util mygettext)

ENDIF (LOBBY_C)

################################################################################

IF (LOBBY_S)

    ADD_LIBRARY(lobby_s STATIC ${SOURCES_LOBBY_S})
    TARGET_LINK_LIBRARIES(lobby_s s25util mygettext mysqlclient)

ENDIF (LOBBY_S)

################################################################################
