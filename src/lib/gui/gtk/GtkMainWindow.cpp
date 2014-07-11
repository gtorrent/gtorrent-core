#include "GtkMainWindow.hpp"

GtkMainWindow::GtkMainWindow()
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(1280, 720);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_title("gTorrent");
	header->set_show_close_button(true);

	this->set_titlebar(*header);

	this->show_all();
}

void GtkMainWindow::on_destroy()
{
}