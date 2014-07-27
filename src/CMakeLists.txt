# Platform-specific source file
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	SET(PLATFORM_SRC Platform_linux.cpp)
ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	SET(PLATFORM_SRC Platform_win.cpp)
ENDIF ()

# NOTE For now I'll not use ${PROJECT_NAME}
# Bear with me here.
INCLUDE_DIRECTORIES (
	../include/gtorrent
	${Boost_INCLUDE_DIRS}
	${LIBTORRENT_INCLUDE_DIRS}
	)

ADD_DEFINITIONS (
	${LIBTORRENT_CFLAGS}
	)

ADD_LIBRARY ( gtorrent
	Core.cpp
	Torrent.cpp
	Log.cpp
	${PLATFORM_SRC}
	)

TARGET_LINK_LIBRARIES ( gtorrent
	${CMAKE_SOURCE_DIRECTORY}/libs/libtorrent.a
	${Boost_LIBRARIES}
	${LIBTORRENT_LIBRARIES}
	)      
