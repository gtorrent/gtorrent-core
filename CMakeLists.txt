############################################################

cmake_minimum_required(VERSION 2.8)

if (COMMAND cmake_policy)
 	cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

############################################################

project (gTorrent)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

#find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories (
	${Boost_INCLUDE_DIR}
)

set (boost_LIBS
	boost_system
	#boost_threadls 
)

set (gTorrent_SRCS
	src/main.cpp
	src/Application.cpp
	src/lib/core/Core.cpp
	src/lib/core/TorrentEngine.cpp
)

set (gTorrent_LIBS
	${boost_LIBS}
	torrent-rasterbar
)

add_executable (gTorrent ${gTorrent_SRCS})
target_link_libraries (gTorrent ${gTorrent_LIBS})
