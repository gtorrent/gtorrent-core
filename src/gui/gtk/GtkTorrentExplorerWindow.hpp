#ifndef GTKTORRENTEXPLORERWINDOW_HPP
#define GTKTORRENTEXPLORERWINDOW_HPP

#include <gtkmm/headerbar.h>
#include "GtkWindow.hpp"

class GtkTorrentExplorerWindow : gt::GtkWindow
{
private:
	Gtk::HeaderBar *m_header;
public:
	GtkTorrentExplorerWindow();
};

#endif