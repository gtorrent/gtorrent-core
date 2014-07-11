#ifndef GTKMAINWINDOW_HPP
#define GTKMAINWINDOW_HPP

#include <gtkmm/headerbar.h>
#include <gtkmm/window.h>

class GtkMainWindow : public Gtk::Window
{
private:
	Gtk::HeaderBar *header;
public:
	GtkMainWindow();
	void on_destroy();
};

#endif