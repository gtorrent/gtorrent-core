AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -std=c++11 -g -I../src/lib $(libgtkmm_CFLAGS) $(libglibmm_CFLAGS)
AM_LDFLAGS = $(libgtkmm_LIBS) $(libglibmm_LIBS)

VPATH = ../src
bin_PROGRAMS = gTorrent
gTorrent_SOURCES = main.cpp Application.cpp lib/core/Core.cpp lib/core/TorrentEngine.cpp lib/core/Torrent.cpp lib/gui/gtk/Gtk.cpp lib/gui/gtk/GtkMainWindow.cpp
