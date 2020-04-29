cmake_minimum_required(VERSION 3.8)
project(liblobby)

set(SOURCES_LOBBY src/LobbyMessage.cpp    include/liblobby/LobbyMessage.h
                                          include/liblobby/LobbyMessages.h
                                          include/liblobby/LobbyMessageInterface.h
                  src/LobbyInterface.cpp  include/liblobby/LobbyInterface.h
                  src/LobbyPlayerInfo.cpp include/liblobby/LobbyPlayerInfo.h
                                          include/liblobby/LobbyPlayerList.h
                  src/LobbyServerInfo.cpp include/liblobby/LobbyServerInfo.h
                                          include/liblobby/LobbyServerList.h
)

set(SOURCES_LOBBY_C src/LobbyClient.cpp   include/liblobby/LobbyClient.h)

add_library(lobby STATIC EXCLUDE_FROM_ALL ${SOURCES_LOBBY})
target_include_directories(lobby PUBLIC include)
target_link_libraries(lobby PUBLIC s25util::common s25util::log s25util::network PRIVATE mygettext)
target_compile_features(lobby PUBLIC cxx_std_14)
set_target_properties(lobby PROPERTIES CXX_EXTENSIONS OFF)

include(EnableWarnings)
enable_warnings(lobby)

add_library(lobby_c STATIC EXCLUDE_FROM_ALL ${SOURCES_LOBBY_C})
target_link_libraries(lobby_c PUBLIC lobby PRIVATE mygettext)

enable_warnings(lobby_c)

if(ClangFormat_FOUND)
    add_clangFormat_files(${SOURCES_LOBBY_C} ${SOURCES_LOBBY})
endif()

include(RttrTestingCfg)
if(BUILD_TESTING)
    add_subdirectory(tests)
    if(ClangFormat_FOUND)
        add_ClangFormat_folder(tests TRUE)
    endif()
endif()
