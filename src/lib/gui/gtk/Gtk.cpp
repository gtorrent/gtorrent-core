#include "Gtk.hpp"
#include <gtkmm/application.h>
#include "GtkMainWindow.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	Gtk::Main kit;

	/* Optional, could be a config option for Gtk users */

	Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
	settings->property_gtk_application_prefer_dark_theme() = true;

	Glib::RefPtr<Gtk::Application> gapp = Gtk::Application::create(argc, argv, "org.gtorrent.gtorrent");
	GtkMainWindow *wnd = new GtkMainWindow();
}

void GuiGtk::update()
{
	while (Gtk::Main::events_pending())
	{
		/* Main GTK Loop */
		Gtk::Main::iteration();
	}
}