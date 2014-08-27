# Select platform specific source file(s)
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(PLATFORM_SRC Platform_linux.cpp)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	SET(PLATFORM_SRC Platform_win.cpp)
# TODO Add Mac OS
ENDIF ()

# NOTE For now I'll not use ${PROJECT_NAME}
# Bear with me here.
INCLUDE_DIRECTORIES (
	../include/gtorrent
	${Boost_INCLUDE_DIRS}
	${LIBTORRENT_INCLUDE_DIRS}
	)

# Set up Version.hpp
CONFIGURE_FILE (utils/Version.hpp.in ${PROJECT_BINARY_DIR}/Version.hpp @ONLY)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

ADD_DEFINITIONS (
	${LIBTORRENT_CFLAGS}
	)

ADD_LIBRARY ( gtorrent
	${PLATFORM_SRC}
	Core.cpp
	Torrent.cpp
	Log.cpp
	Peer.cpp
	Settings.cpp
	)

TARGET_LINK_LIBRARIES (
	gtorrent
	${CMAKE_SOURCE_DIRECTORY}/libs/libtorrent.a
	${Boost_LIBRARIES}
	${LIBTORRENT_LIBRARIES}
	)

INSTALL (TARGETS gtorrent ARCHIVE DESTINATION ${PREFIX}/lib/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
