AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -std=c++11 -g -I../src/lib -I../src $(libgtkmm_CFLAGS) $(libglibmm_CFLAGS)
AM_LDFLAGS = $(libgtkmm_LIBS) $(libglibmm_LIBS)

VPATH = ../src
bin_PROGRAMS = gTorrent

gTorrent_SOURCES = main.cpp Application.cpp lib/core/Core.cpp lib/core/OSHelper.cpp lib/core/TorrentEngine.cpp lib/core/Torrent.cpp lib/shared/Log.cpp lib/gui/core/Base.cpp lib/gui/gtk/Gtk.cpp lib/gui/gtk/GtkWindow.cpp lib/gui/gtk/GtkTorrentColumns.cpp lib/gui/gtk/GtkTreeTorrentView.cpp lib/gui/gtk/GtkMainWindow.cpp lib/gui/gtk/GtkTorrentExplorerWindow.cpp