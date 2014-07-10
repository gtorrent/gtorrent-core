AC_INIT([gTorrent], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX(g++)

AC_CHECK_FILE([src/main.cpp])
AC_CHECK_FILE([src/Application.cpp])
AC_CHECK_FILE([src/lib/core/Core.cpp])
AC_CHECK_FILE([src/lib/core/TorrentEngine.cpp])
AC_CHECK_FILE([src/lib/core/Torrent.cpp])

AC_CHECK_LIB([boost_system], [main])
AC_CHECK_LIB([torrent-rasterbar], [main])

AC_CONFIG_FILES([Makefile] [bin/Makefile])
AC_OUTPUT
