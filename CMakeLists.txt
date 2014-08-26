############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

PROJECT (gtorrent)

SET (GTORRENT_VERSION_MAJOR 0)
SET (GTORRENT_VERSION_MINOR 0)
SET (GTORRENT_VERSION_PATCH 2)
SET (GTORRENT_VERSION ${GTORRENT_VERSION_MAJOR}.${GTORRENT_VERSION_MINOR}.${GTORRENT_VERSION_PATCH})

############################################################

# Options
OPTION (ENABLE_TESTEXCUTABLE     "Enable the compilation of a simple test" OFF)
OPTION (USE_LOGGING              "Turns logging on/off"                    ON)
OPTION (Boost_USE_STATIC_LIBS    "Use static version of Boost libraries."  ON)
OPTION (Boost_USE_MULTITHREADED  "Use multithreaded version of Boost."     ON)  
OPTION (Boost_USE_STATIC_RUNTIME "Use Boost static runtime."               OFF) 

IF (USE_LOGGING)
ADD_DEFINITIONS(-DUSE_LOGGING=1)
ENDIF()

# Configure version into utils/Version.hpp
SET (VERSION ${GTORRENT_VERSION_MAJOR}.${GTORRENT_VERSION_MINOR}.${GTORRENT_VERSION_PATCH})

# Set compiler flags
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

# Fuck libboost for not providing boost.pc or fucking something
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

# Find libraries the proper way
INCLUDE (FindPkgConfig)
PKG_SEARCH_MODULE (LIBTORRENT REQUIRED libtorrent-rasterbar)

# Compiler options
LINK_DIRECTORIES (
	${Boost_LIBRARY_DIRS}
	${LIBTORRENT_LIBRARY_DIRS}
	${CMAKE_SOURCE_DIRECTORY}
	)

ADD_DEFINITIONS (
	${LIBTORRENT_CFLAGS}
	)

INCLUDE_DIRECTORIES (
	include/gtorrent
	${Boost_INCLUDE_DIRS}
	${LIBTORRENT_INCLUDE_DIRS}
	)

# Create test executable if enabled
IF (ENABLE_TESTEXCUTABLE)
	ADD_EXECUTABLE ( gTest
		EXCLUDE_FROM_ALL
		test/main.cpp
		)

	# Make sure libgtorrent.a is built before gTest.exe
	ADD_DEPENDENCIES (gTest gtorrent)

	# Add and move test torrents
	ADD_CUSTOM_TARGET (copytorrents
		COMMENT "Copying torrents"
		)
	FILE (GLOB Torrents ${PROJECT_SOURCE_DIR}/test/*.torrent)
	FOREACH (Torrent ${Torrents})
		ADD_CUSTOM_COMMAND(TARGET copytorrents 
			COMMAND ${CMAKE_COMMAND} -E copy ${Torrent} ${CMAKE_BINARY_DIR}/${Torrent_NAME}
			COMMENT "Copying ${Torrent}"
			)
	ENDFOREACH()
	ADD_DEPENDENCIES(gTest copytorrents)

	TARGET_LINK_LIBRARIES ( gTest
		${CMAKE_BINARY_DIR}/libgtorrent.a
		${Boost_LIBRARIES}
		${LIBTORRENT_LIBRARIES}
		)      
ENDIF (ENABLE_TESTEXCUTABLE)

ADD_SUBDIRECTORY (src)
