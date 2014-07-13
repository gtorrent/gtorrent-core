AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -std=c++11 -g -I../src/lib -I../src $(libgtkmm_CFLAGS) $(libglibmm_CFLAGS) $(libtorrent_CFLAGS)
AM_LDFLAGS = $(libgtkmm_LIBS) $(libglibmm_LIBS) $(libtorrent_LIBS)

VPATH = ../src
bin_PROGRAMS = gTorrent

gTorrent_SOURCES = main.cpp \
	Application.cpp \
	lib/core/Core.cpp \
	lib/core/OSHelper.cpp \
	lib/core/TorrentEngine.cpp \
	lib/core/Torrent.cpp \
	lib/shared/Log.cpp \
	gui/core/Base.cpp \
	gui/gtk/Gtk.cpp \
	gui/gtk/GtkWindow.cpp \
	gui/gtk/GtkTorrentColumns.cpp \
	gui/gtk/GtkTreeTorrentView.cpp \
	gui/gtk/GtkMainWindow.cpp \
	gui/gtk/GtkTorrentExplorerWindow.cpp