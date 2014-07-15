############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if (COMMAND cmake_policy)
 cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

PROJECT (gtorrent)

SET (GTORRENT_VERSION_MAJOR 0)                                                                                                                                                                                                                 
SET (GTORRENT_VERSION_MINOR 0)
SET (GTORRENT_VERSION_PATCH 0)

############################################################

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

# Fuck liboost for not providing boost.pc or fucking something
SET (Boost_USE_STATIC_LIBS ON) 
SET (Boost_USE_MULTITHREADED ON)  
SET (Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

# Find libraries the proper way
INCLUDE (FindPkgConfig)
PKG_SEARCH_MODULE(LIBTORRENT REQUIRED libtorrent-rasterbar)
PKG_SEARCH_MODULE(LIBGTKMM REQUIRED gtkmm-3.0)
PKG_SEARCH_MODULE(LIBGLIBMM REQUIRED glibmm-2.4)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIRS} )

LINK_DIRECTORIES (
        ${Boost_LIBRARY_DIRS}
        ${LIBTORRENT_LIBRARY_DIRS}
        ${LIBGTKMM_LIBRARY_DIRS}
        ${LIBGLIBMM_LIBRARY_DIRS}
)

ADD_EXECUTABLE ( ${PROJECT_NAME}
        src/Application.cpp
        src/gui/core/Base.cpp
        src/gui/gtk/Gtk.cpp
        src/gui/gtk/GtkAddMagnetLinkWindow.cpp
        src/gui/gtk/GtkMainWindow.cpp
        src/gui/gtk/GtkTreeTorrentView.cpp
        src/core/Core.cpp
        src/core/Torrent.cpp
        src/core/Log.cpp
        src/main.cpp
)

ADD_DEFINITIONS (
        ${LIBTORRENT_CFLAGS}
        ${LIBGLIBMM_CFLAGS}
        ${LIBGTKMM_CFLAGS}
)

INCLUDE_DIRECTORIES (
        src/gui/
        src/
        ${Boost_INCLUDE_DIRS}
        ${LIBTORRENT_INCLUDE_DIRS}
        ${LIBGTKMM_INCLUDE_DIRS}
        ${LIBGLIBMM_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES ( ${PROJECT_NAME}
        ${Boost_LIBRARIES}
        ${LIBTORRENT_LIBRARIES}
        ${LIBGTKMM_LIBRARIES}
        ${LIBGLIBMM_LIBRARIES}
)      
