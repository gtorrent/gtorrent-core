#include "GtkTorrentExplorerWindow.hpp"
#include <gtkmm/main.h>

GtkTorrentExplorerWindow::GtkTorrentExplorerWindow()
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(900, 600);
	this->show_all();
}