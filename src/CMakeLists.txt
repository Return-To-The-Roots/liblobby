get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)

SET(HEADER_PATH ${includePath}/liblobby)
SET(SOURCES_LOBBY LobbyMessage.cpp          ${HEADER_PATH}/LobbyMessage.h
                  ${HEADER_PATH}/LobbyMessages.h
                                            ${HEADER_PATH}/LobbyMessageInterface.h
                  LobbyInterface.cpp        ${HEADER_PATH}/LobbyInterface.h
                  LobbyPlayerInfo.cpp       ${HEADER_PATH}/LobbyPlayerInfo.h
                  ${HEADER_PATH}/LobbyPlayerList.h
                  LobbyServerInfo.cpp       ${HEADER_PATH}/LobbyServerInfo.h
                  ${HEADER_PATH}/LobbyServerList.h
   )

SET(SOURCES_LOBBY_C ${SOURCES_LOBBY} LobbyClient.cpp ${HEADER_PATH}/LobbyClient.h)
SET(SOURCES_LOBBY_S ${SOURCES_LOBBY} LobbyServer.cpp ${HEADER_PATH}/LobbyServer.h
                                     LobbyPlayer.cpp ${HEADER_PATH}/LobbyPlayer.h
                                     MySQL.cpp MySQL.h)

################################################################################

INCLUDE_DIRECTORIES(${HEADER_PATH})

IF (RTTR_BUILD_LOBBY_CLIENT)
	find_package(Boost 1.55 REQUIRED)
	INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})
    ADD_LIBRARY(lobby_c STATIC ${SOURCES_LOBBY_C})
    target_include_directories(lobby_c PUBLIC ${includePath})
    TARGET_LINK_LIBRARIES(lobby_c s25util mygettext)
    if(ClangFormat_FOUND)
        add_clangFormat_files(${SOURCES_LOBBY_C})
    endif()

ENDIF ()

################################################################################

IF (RTTR_BUILD_LOBBY_SERVER)
	FIND_PACKAGE(MYSQL REQUIRED)
	INCLUDE_DIRECTORIES(SYSTEM ${MYSQL_INCLUDE_DIRS})
    ADD_LIBRARY(lobby_s STATIC ${SOURCES_LOBBY_S})
    target_include_directories(lobby_s PUBLIC ${includePath})
    
    TARGET_LINK_LIBRARIES(lobby_s s25util mygettext)
    target_link_libraries(lobby_s SYSTEM ${MYSQL_LIBRARIES})
	add_executable(lobbyServer lobbyServerApp.cpp)
	target_link_libraries(lobbyServer lobby_s)
endif()

################################################################################
