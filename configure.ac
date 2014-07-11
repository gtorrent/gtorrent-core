AC_INIT([gTorrent], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX(g++)

AC_CHECK_FILE([src/main.cpp])
AC_CHECK_FILE([src/Application.cpp])
AC_CHECK_FILE([src/lib/core/Core.cpp])
AC_CHECK_FILE([src/lib/core/TorrentEngine.cpp])
AC_CHECK_FILE([src/lib/core/Torrent.cpp])

# GTK Related GUI Files
AC_CHECK_FILE([src/lib/gui/core/Base.cpp])
AC_CHECK_FILE([src/lib/gui/gtk/Gtk.cpp])
AC_CHECK_FILE([src/lib/gui/gtk/GtkMainWindow.cpp])

AC_CHECK_LIB([boost_system], [main])
AC_CHECK_LIB([torrent-rasterbar], [main])

# GTK pkg-config
PKG_CHECK_MODULES([libgtkmm], [gtkmm-3.0])
PKG_CHECK_MODULES([libglibmm], [glibmm-2.4])

AC_CONFIG_FILES([Makefile] [bin/Makefile])
AC_OUTPUT
