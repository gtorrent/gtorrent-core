############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

if (COMMAND cmake_policy)
 cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

PROJECT (gtorrent)

############################################################

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

INCLUDE (FindPkgConfig)
PKG_SEARCH_MODULE(LIBTORRENT REQUIRED libtorrent-rasterbar)

SET (gTorrent_LIBS
        ${BOOST_LIBS}
        torrent-rasterbar
)

ADD_EXECUTABLE ( ${PROJECT_NAME}
        src/main.cpp
        src/Application.cpp
        src/lib/core/Core.cpp
        src/lib/core/TorrentEngine.cpp
        src/lib/core/Torrent.cpp
)

INCLUDE_DIRECTORIES (
        src/
        ${Boost_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES (
        ${PROJECT_NAME}
        ${gTorrent_LIBS}
)      
