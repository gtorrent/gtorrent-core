############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

PROJECT (gtorrent)

SET (GTORRENT_VERSION_MAJOR 0)
SET (GTORRENT_VERSION_MINOR 0)
SET (GTORRENT_VERSION_PATCH 2)

############################################################

# Options
OPTION (ENABLE_TESTEXCUTABLE "Enable the compilation of a simple test" OFF)

# Configure version into utils/Version.hpp
SET (VERSION ${GTORRENT_VERSION_MAJOR}.${GTORRENT_VERSION_MINOR}.${GTORRENT_VERSION_PATCH})
CONFIGURE_FILE (src/utils/Version.hpp.in Version.hpp @ONLY)

# Set compiler flags
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

# Fuck libboost for not providing boost.pc or fucking something
SET (Boost_USE_STATIC_LIBS ON) 
SET (Boost_USE_MULTITHREADED ON)  
SET (Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

# Find libraries the proper way
INCLUDE (FindPkgConfig)
PKG_SEARCH_MODULE (LIBTORRENT REQUIRED libtorrent-rasterbar)

# To find version
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

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

INSTALL (TARGETS gtorrent DESTINATION ${PREFIX}/lib/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
