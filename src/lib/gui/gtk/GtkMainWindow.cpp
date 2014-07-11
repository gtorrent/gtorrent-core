#include "GtkMainWindow.hpp"
#include <gtkmm/button.h>

GtkMainWindow::GtkMainWindow()
{
	this->set_position(Gtk::WIN_POS_CENTER);
	this->set_default_size(1280, 720);

	header = Gtk::manage(new Gtk::HeaderBar());
	header->set_title("gTorrent");
	header->set_show_close_button(true);

	Gtk::Button *btn = new Gtk::Button();
	btn->set_label("Add");
	header->add(*btn);

	this->set_titlebar(*header);

	this->show_all();
}

void GtkMainWindow::on_destroy()
{
}