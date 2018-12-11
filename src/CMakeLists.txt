get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)

set(HEADER_PATH ${includePath}/liblobby)
set(SOURCES_LOBBY LobbyMessage.cpp      ${HEADER_PATH}/LobbyMessage.h
                                        ${HEADER_PATH}/LobbyMessages.h
                                        ${HEADER_PATH}/LobbyMessageInterface.h
                  LobbyInterface.cpp    ${HEADER_PATH}/LobbyInterface.h
                  LobbyPlayerInfo.cpp   ${HEADER_PATH}/LobbyPlayerInfo.h
                                        ${HEADER_PATH}/LobbyPlayerList.h
                  LobbyServerInfo.cpp   ${HEADER_PATH}/LobbyServerInfo.h
                                        ${HEADER_PATH}/LobbyServerList.h
)

set(SOURCES_LOBBY_C LobbyClient.cpp ${HEADER_PATH}/LobbyClient.h)
set(SOURCES_LOBBY_S LobbyServer.cpp ${HEADER_PATH}/LobbyServer.h
                    LobbyPlayer.cpp ${HEADER_PATH}/LobbyPlayer.h
                    MySQL.cpp MySQL.h)

add_library(lobby STATIC EXCLUDE_FROM_ALL ${SOURCES_LOBBY})
target_include_directories(lobby PRIVATE ${HEADER_PATH} PUBLIC ${includePath})
target_link_libraries(lobby PUBLIC s25util PRIVATE mygettext)
include(EnableWarnings)
enable_warnings(lobby)

add_library(lobby_c STATIC EXCLUDE_FROM_ALL ${SOURCES_LOBBY_C})
target_link_libraries(lobby_c PUBLIC lobby PRIVATE mygettext)
if(ClangFormat_FOUND)
    add_clangFormat_files(${SOURCES_LOBBY_C} ${SOURCES_LOBBY})
endif()

if(RTTR_BUILD_LOBBY_SERVER)
  find_package(MYSQL REQUIRED)
  add_library(lobby_s STATIC ${SOURCES_LOBBY_S})
  target_include_directories(lobby_s SYSTEM PRIVATE ${MYSQL_INCLUDE_DIRS})
  target_link_libraries(lobby_s PUBLIC lobby ${MYSQL_LIBRARIES})
  add_executable(lobbyServer lobbyServerApp.cpp)
  target_link_libraries(lobbyServer PRIVATE lobby_s)
endif()
