################################################################################
### $Id: CMakeLists.txt 6460 2010-05-31 11:42:38Z FloSoft $
################################################################################

IF ( NOT "${COMPILEFOR}" STREQUAL "windows" )
	IF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
		ADD_DEFINITIONS(-fPIC)
	ENDIF ("${CROSSCOMPILE}" STREQUAL "1" OR "${COMPILEARCH}" STREQUAL "x86_64" )
ENDIF ( NOT "${COMPILEFOR}" STREQUAL "windows" )

################################################################################

SET(SOURCES_LOBBY LobbyMessage.cpp          LobbyMessage.h
                  LobbyMessages.cpp         LobbyMessages.h
                  LobbyMessageInterface.cpp LobbyMessageInterface.h
                  LobbyInterface.cpp        LobbyInterface.h
                  LobbyPlayerInfo.cpp       LobbyPlayerInfo.h
                  LobbyPlayerList.cpp       LobbyPlayerList.h
                  LobbyServerInfo.cpp       LobbyServerInfo.h
                  LobbyServerList.cpp       LobbyServerList.h
   )

################################################################################

INCLUDE_DIRECTORIES("../../" "../../libutil/src/" "../../mygettext/src/")

################################################################################

IF (LOBBY_C)

    ADD_LIBRARY(lobby_c STATIC ${SOURCES_LOBBY} LobbyClient.cpp LobbyClient.h)
    TARGET_LINK_LIBRARIES(lobby_c s25util mygettext)
    
ENDIF (LOBBY_C)

################################################################################

IF (LOBBY_S)

    ADD_LIBRARY(lobby_s STATIC ${SOURCES_LOBBY} LobbyServer.cpp LobbyServer.h  LobbyPlayer.cpp LobbyPlayer.h MySQL.cpp MySQL.h)
    TARGET_LINK_LIBRARIES(lobby_s s25util mygettext mysqlclient)

ENDIF (LOBBY_S)

################################################################################
