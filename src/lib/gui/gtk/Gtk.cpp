#include "Gtk.hpp"
#include <gtkmm/application.h>
#include "GtkMainWindow.hpp"

GuiGtk::GuiGtk(int argc, char **argv)
{
	m_gtkmain = new Gtk::Main(0, 0, false);
	//Gtk::Main kit(argc, argv);

	/* Optional, could be a config option for Gtk users */

	//Glib::RefPtr<Gtk::Settings> settings = Gtk::Settings::get_default();
	//settings->property_gtk_application_prefer_dark_theme() = true;

	//Glib::RefPtr<Gtk::Application> gapp = Gtk::Application::create(argc, argv, "org.gtorrent.gtorrent");
	GtkMainWindow *wnd = new GtkMainWindow();

	//Gtk::Main::run(*wnd);
}

void GuiGtk::update()
{
	while (m_gtkmain->events_pending())
	{
		m_gtkmain->iteration();
	}
	//while (Gtk::Main::events_pending())
	//{
		/* Main GTK Loop */
	//	Gtk::Main::iteration();
	//}
}