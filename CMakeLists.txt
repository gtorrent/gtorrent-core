############################################################

cmake_minimum_required(VERSION 2.8)

if (COMMAND cmake_policy)
 	cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

############################################################

project (gTorrent)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

include_directories (
	# Extra include direcotires here
)

set (gTorrent_SRCS
	src/main.cpp
	src/Application.cpp
)

set (gTorrent_LIBS
	# Link libraries here
)

add_executable (gTorrent ${gTorrent_SRCS})
#target_link_libraries (gTorrent ${am4_LIBS})
